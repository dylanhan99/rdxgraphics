#include <pch.h>
#include "BoundingVolume.h"
#include "ECS/EntityManager.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Components/Camera.h"

void BoundingVolume::SetBVType(BV bvType)
{
	if (m_BVType == bvType)
		return;

	glm::vec3 offset = RemoveBV();
	m_BVType = bvType;
	if (m_BVType == BV::NIL)
		return;

	SetupBV(offset);
}

void BoundingVolume::SetupBV(glm::vec3 offset) const
{
	// Get the BV& component and 
	entt::entity const handle = GetEntityHandle();
#define _RX_X(Klass) case BV::Klass: { EntityManager::AddComponent<Klass##BV>(handle); break; }
	switch (m_BVType)
	{
		RX_DO_ALL_BV_ENUM;
	default:
		RX_ASSERT(false, "How did you get here");
		break;
	}
#undef _RX_X
}

glm::vec3 BoundingVolume::RemoveBV()
{
	m_BVType = BV::NIL;
	return glm::vec3();
}

void BaseBV::SetDirtyBV() const
{
	auto const& handle = GetEntityHandle();
	if (!EntityManager::HasEntity(handle))
		return;
	if (EntityManager::HasComponent<BoundingVolume::Dirty>(handle))
		return;
	
	EntityManager::AddComponent<BoundingVolume::Dirty>(handle);
}

// Actually i dont think we even need to bother with this 
// because m_Points are all in world space
// Ok actually we use this to cache all 8 line edges' xforms
void FrustumBV::UpdateXform() 
{
	auto CalcXform = 
		[](glm::vec3 const& A, glm::vec3 const& B) -> glm::mat4
		{
			glm::vec3 from = RayPrimitive::DefaultDirection;
			glm::vec3 to = B - A;

			glm::quat quat = glm::rotation(from, glm::normalize(to));
			return glm::translate(A) * glm::scale(glm::vec3(glm::length(to))) * glm::mat4_cast(quat);
		};

	// Near plane, TL > BL > BR > TR
	// Far plane,  TL > BL > BR > TR
	// m_Points

	// Forward edges
	m_Xforms[0] = CalcXform(m_Points[0], m_Points[4]); // TL
	m_Xforms[1] = CalcXform(m_Points[1], m_Points[5]); // BL
	m_Xforms[2] = CalcXform(m_Points[2], m_Points[6]); // BR
	m_Xforms[3] = CalcXform(m_Points[3], m_Points[7]); // TR

	// Near plane
	m_Xforms[4] = CalcXform(m_Points[0], m_Points[1]); // NL
	m_Xforms[5] = CalcXform(m_Points[1], m_Points[2]); // NB
	m_Xforms[6] = CalcXform(m_Points[2], m_Points[3]); // NR
	m_Xforms[7] = CalcXform(m_Points[3], m_Points[0]); // NT

	// Far plane
	m_Xforms[8]  = CalcXform(m_Points[4], m_Points[5]); // FL
	m_Xforms[9]  = CalcXform(m_Points[5], m_Points[6]); // FB
	m_Xforms[10] = CalcXform(m_Points[6], m_Points[7]); // FR
	m_Xforms[11] = CalcXform(m_Points[7], m_Points[4]); // FT
}

void FrustumBV::RecalculateBV()
{
	entt::entity const handle = GetEntityHandle();
	if (!EntityManager::HasComponent<Camera>(handle))
	{
		RX_WARN("FrustumBV is lacking a Camera component - entt::{}", (uint32_t)handle);
		return;
	}

	Camera& camera = EntityManager::GetComponent<Camera>(handle);
	glm::mat4 const invMatrix = glm::inverse(camera.GetProjMatrix() * camera.GetViewMatrix());

	for (uint32_t i = 0; i < 8; ++i)
	{
		glm::vec4& curr = m_Points[i];
		curr = invMatrix * NDCPoints[i];
		curr /= curr.w; // perspective division
	}

	// Setting up the plane equations now that we have the points
	{
		// 3 points and normal are obvious, 
		// but get the D via normalized normal DOT P0 (any of the points)
		auto const& fPoints = m_Points;
		// not really a center, just figuratively a point inside the frustum
		glm::vec3 center{};
		for (auto const& point : fPoints)
			center += static_cast<glm::vec3>(point);
		center /= fPoints.size();
		auto MakePlaneEquation =
			[&center](glm::vec3 const& A, glm::vec3 const& B, glm::vec3 const& C) -> glm::vec4
			{
				glm::vec3 normal = glm::normalize(glm::cross(B - A, C - A));

				// Flip the normal if it's pointing the wrong way
				// Our standard is an inward pointing frustum
				if (glm::dot(normal, center - A) < 0.f)
					normal = -normal;

				float d = glm::dot(normal, C);
				return glm::vec4{ normal, d };
			};

		auto& planeEquations = GetPlaneEquations();
#define _RX_XXX(i, A, B, C) planeEquations[i] = MakePlaneEquation(fPoints[A], fPoints[B], fPoints[C])
		_RX_XXX(0, 4, 5, 6);
		_RX_XXX(1, 3, 2, 1);
		_RX_XXX(2, 0, 1, 5);
		_RX_XXX(3, 7, 6, 2);
		_RX_XXX(4, 3, 0, 4);
		_RX_XXX(5, 1, 2, 6);
#undef _RX_XXX
	}
}

void AABBBV::RecalculateBV()
{
	// "Original"
	Model const& model = EntityManager::GetComponent<const Model>(GetEntityHandle());
	auto& obj = RenderSystem::GetObjekt(model.GetMesh());
	AABBBV const& defaultBV = obj.GetDefaultAABBBV();

	glm::vec3 const& defaultCenter = defaultBV.GetOffset();
	glm::vec3 const& defaultHalfExtents = defaultBV.GetHalfExtents();

	Xform const& xform = EntityManager::GetComponent<const Xform>(GetEntityHandle());
	glm::vec3 const& scl = xform.GetScale();
	glm::mat4 const rot = xform.GetRotationMatrix();

	glm::vec3 newCenter{ 0.f };
	if (defaultCenter != glm::vec3{ 0.f })
		newCenter = /*glm::translate(xform.GetTranslate()) **/ rot * glm::scale(scl) * glm::vec4{ defaultCenter, 1.f };

	glm::vec3 newHalfSize{};
	for (int i = 0; i < 3; ++i)
	{
		newHalfSize[i] =
			glm::abs(rot[i][0]) * defaultHalfExtents[0] * scl[0] +
			glm::abs(rot[i][1]) * defaultHalfExtents[1] * scl[1] +
			glm::abs(rot[i][2]) * defaultHalfExtents[2] * scl[2];
	}

	GetHalfExtents() = newHalfSize;
	GetOffset() = newCenter;
}

void SphereBV::RecalculateBV()
{
	switch (Algorithm)
	{
	case Algo::Ritter:
	{

		break;
	}
	case Algo::Larsson:
		break;
	case Algo::PCA:
		break;
	default: break;
	}
}