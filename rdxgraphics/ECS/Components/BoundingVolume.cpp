#include <pch.h>
#include "BoundingVolume.h"
#include "ECS/EntityManager.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Components/Camera.h"
#include "Utils/IntersectionTests.h"

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

void BoundingVolume::SetDirty() const
{
#define _RX_X(Klass)										  \
case BV::Klass: {											  \
	EntityManager::GetComponent<Klass##BV>(GetEntityHandle()).SetDirty();\
	break;													  \
}
	switch (m_BVType)
	{
		RX_DO_ALL_BV_ENUM;
	default:
		break;
	}
#undef _RX_X
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

void BaseBV::SetDirtyXform() const
{
	auto const& handle = GetEntityHandle();
	if (!EntityManager::HasEntity(handle))
		return;
	if (EntityManager::HasComponent<BoundingVolume::DirtyXform>(handle))
		return;

	EntityManager::AddComponent<BoundingVolume::DirtyXform>(handle);
}

void BaseBV::SetDirtyBV() const
{
	auto const& handle = GetEntityHandle();
	if (!EntityManager::HasEntity(handle))
		return;
	if (EntityManager::HasComponent<BoundingVolume::DirtyBV>(handle))
		return;
	
	EntityManager::AddComponent<BoundingVolume::DirtyBV>(handle);
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
			return glm::translate(A) * glm::mat4_cast(quat) * glm::scale(glm::vec3(glm::length(to)));
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
	Xform& modelXform = EntityManager::GetComponent<Xform>(GetEntityHandle());
	Rxuid const meshID = EntityManager::GetComponent<Model>(GetEntityHandle()).GetMesh();
	auto& objekt = RenderSystem::GetObjekt(meshID);
	auto const& points = objekt.GetVBData<VertexBasic::Position>();

	switch (Algorithm)
	{
	case Algo::Ritter:
	{
		// Get 6 random points and xform them
		// Get the most min and most max x-y-z out of them, find the centroid 
		// this will be your starting sphere
		std::mt19937 gen(24); // Fixed seed
		std::uniform_int_distribution<> distribution{ 0, (int)points.size() - 1 };
		std::vector<glm::vec3> startingPoints{}; startingPoints.resize(6);
		for (int i = 0; i < 6; ++i)
			startingPoints[i] = glm::vec3{ modelXform.GetXform() * glm::vec4{ points[distribution(gen)], 1.f } };

		glm::vec3 finalCentroid{};
		float finalRadius{};
		// SphereFromDistantPoints
		{
			size_t min{}, max{};
			Intersection::MostSeparatedPointsOnAABB(startingPoints, min, max);

			// Set up sphere to just encompass these two points
			finalCentroid = (startingPoints[min] + startingPoints[max]) * 0.5f;
			finalRadius = glm::dot(startingPoints[max] - finalCentroid, startingPoints[max] - finalCentroid);
			finalRadius = glm::sqrt(finalRadius);
		}

		// Now, for every single point, xform them and compare, if it is in or out of sphere.
		// If in, continue
		// If out, then expand the sphere, find the new centroid and radius
		// next point.
		for (auto point : points)
		{
			point = glm::vec3{ modelXform.GetXform() * glm::vec4{ point, 1.f } };

			int col = Intersection::PointSphereTest(
				point,
				finalCentroid, finalRadius
			);

			if (col != -1)
				continue;

			glm::vec3 d = point - finalCentroid;
			float dist2 = glm::dot(d, d);

			if (dist2 > glm::dot(finalRadius, finalRadius))
			{
				float dist = glm::sqrt(dist2);
				float newRadius = (finalRadius + dist) * 0.5f;
				float k = (newRadius - finalRadius) / dist;

				finalRadius = newRadius;
				finalCentroid += d * k;
			}
		}

		GetRadius() = finalRadius;
		SetPosition(finalCentroid);
		break;
	}
	case Algo::Larsson:
	{
		// EPOS-6 direction dictionary: ±X, ±Y, ±Z
		std::vector<glm::vec3> directionDictionary = {
			{1, 0, 0}, {-1, 0, 0},
			{0, 1, 0}, {0, -1, 0},
			{0, 0, 1}, {0, 0, -1}
		};


		auto pointsCopy = points;
		for (auto& v : pointsCopy) // This is so bad lmfao
			v = glm::vec3{ modelXform.GetXform() * glm::vec4{ v, 1.f } };

		std::vector<glm::vec3> extremalPoints;
		for (const auto& dir : directionDictionary) {
			float maxProj = -std::numeric_limits<float>::infinity();
			float minProj = std::numeric_limits<float>::infinity();
			glm::vec3 pMax, pMin;
			for (auto const& v : pointsCopy) {
				float proj = glm::dot(v, dir);
				if (proj > maxProj) {
					maxProj = proj;
					pMax = v;
				}
				if (proj < minProj) {
					minProj = proj;
					pMin = v;
				}
			}
			extremalPoints.push_back(pMax);
			extremalPoints.push_back(pMin);
		}

		// Step 2: Find farthest pair among extremal points
		float maxDistance = 0.0f;
		glm::vec3 p1, p2;
		for (size_t i = 0; i < extremalPoints.size(); ++i) {
			for (size_t j = i + 1; j < extremalPoints.size(); ++j) {
				float dist = glm::distance(extremalPoints[i], extremalPoints[j]);
				if (dist > maxDistance) {
					maxDistance = dist;
					p1 = extremalPoints[i];
					p2 = extremalPoints[j];
				}
			}
		}
		// Step 3: Initial sphere from farthest pair
		glm::vec3 center = (p1 + p2) * 0.5f;
		float radius = maxDistance * 0.5f;
		// Step 4: Grow to include all points
		for (const auto& v : pointsCopy) {
			float dist = glm::distance(center, v);
			if (dist > radius) {
				float newRadius = (radius + dist) * 0.5f;
				glm::vec3 direction = glm::normalize(v - center);
				center += (newRadius - radius) * direction;
				radius = newRadius;
			}
		}

		GetRadius() = radius;
		SetPosition(center);
		break;
	}
	case Algo::PCA:
		break;
	default: break;
	}
}