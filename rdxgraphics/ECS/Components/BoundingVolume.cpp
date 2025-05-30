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

void BaseBV::SetDirty() const
{
	//auto const& handle = GetEntityHandle();
	//if (EntityManager::HasComponent<BoundingVolume::Dirty>(handle))
	//	return;
	//
	//EntityManager::AddComponent<BoundingVolume::Dirty>(handle);
}

// Actually i dont think we even need to bother with this 
// because m_Points are all in world space
// Ok actually we use this to cache all 8 line edges' xforms
void FrustumBV::UpdateXform() 
{
	Xform& xform = EntityManager::GetComponent<Xform>(GetEntityHandle());
	auto CalcXform = 
		[pos = xform.GetTranslate()](glm::vec3 const& A, glm::vec3 const& B) -> glm::mat4
		{
			glm::vec3 from = RayPrimitive::DefaultDirection;
			glm::vec3 to = B - A;

			glm::quat quat = glm::rotation(from, glm::normalize(to));
			return glm::translate(pos + A) * glm::scale(glm::vec3(glm::length(to))) * glm::mat4_cast(quat);
		};

	// Near plane, TL > BL > BR > TR
	// Far plane,  TL > BL > BR > TR
	// m_Points
	m_Xforms[0] = CalcXform(m_Points[0], m_Points[4]); // TL
	m_Xforms[1] = CalcXform(m_Points[1], m_Points[5]); // BL
	m_Xforms[2] = CalcXform(m_Points[2], m_Points[6]); // BR
	m_Xforms[3] = CalcXform(m_Points[3], m_Points[7]); // TR
	m_Xforms[4] = CalcXform(m_Points[0], m_Points[3]); // NT
	m_Xforms[5] = CalcXform(m_Points[1], m_Points[2]); // NB
	m_Xforms[6] = CalcXform(m_Points[4], m_Points[7]); // FT
	m_Xforms[7] = CalcXform(m_Points[5], m_Points[6]); // FB
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
	glm::mat4 const invViewMatrix = glm::inverse(camera.GetViewMatrix());

	for (uint32_t i = 0; i < 8; ++i)
	{
		glm::vec4& curr = m_Points[i];
		curr = invViewMatrix * NDCPoints[i];
		curr /= curr.w; // perspective division
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

	//GetHalfExtents() = newHalfSize;
	GetRadius() =
		newHalfSize.x * newHalfSize.x * newHalfSize.x +
		newHalfSize.y * newHalfSize.y * newHalfSize.y +
		newHalfSize.z * newHalfSize.z * newHalfSize.z;
	GetOffset() = newCenter;
}