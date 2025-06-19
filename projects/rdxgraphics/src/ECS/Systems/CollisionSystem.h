#pragma once
#include "ECS/EntityManager.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/BoundingVolume.h"

class CollisionSystem : public BaseSingleton<CollisionSystem>
{
	RX_SINGLETON_DECLARATION(CollisionSystem);
public:
	static void Update(float dt);

	static BV& GetGlobalBVType() { return g.m_GlobalBVType; }

private:
	BV m_GlobalBVType{ BV::AABB };

public:
	// Default will come here.
	template <typename T, typename U>
	static bool CheckCollision(T const& lhs, U const& rhs)
	{
		return false;
	}
	// All supported intersection tests follow.

	static bool CheckCollision(PointPrimitive const& lhs, PointPrimitive const& rhs);
	//static bool CheckCollision(PointPrimitive const& lhs, RayPrimitive const& rhs);
	static bool CheckCollision(PointPrimitive const& lhs, TrianglePrimitive const& rhs);
	static bool CheckCollision(PointPrimitive const& lhs, PlanePrimitive const& rhs);
	static bool CheckCollision(PointPrimitive const& lhs, AABBPrimitive const& rhs);
	static bool CheckCollision(PointPrimitive const& lhs, SpherePrimitive const& rhs);

	//static bool CheckCollision(RayPrimitive const& lhs, PointPrimitive const& rhs);
	//static bool CheckCollision(RayPrimitive const& lhs, RayPrimitive const& rhs);
	static bool CheckCollision(RayPrimitive const& lhs, TrianglePrimitive const& rhs);
	static bool CheckCollision(RayPrimitive const& lhs, PlanePrimitive const& rhs);
	static bool CheckCollision(RayPrimitive const& lhs, AABBPrimitive const& rhs);
	static bool CheckCollision(RayPrimitive const& lhs, SpherePrimitive const& rhs);

	static bool CheckCollision(TrianglePrimitive const& lhs, PointPrimitive const& rhs);
	static bool CheckCollision(TrianglePrimitive const& lhs, RayPrimitive const& rhs);
	//static bool CheckCollision(TrianglePrimitive const& lhs, TrianglePrimitive const& rhs);
	//static bool CheckCollision(TrianglePrimitive const& lhs, PlanePrimitive const& rhs);
	//static bool CheckCollision(TrianglePrimitive const& lhs, AABBPrimitive const& rhs);
	//static bool CheckCollision(TrianglePrimitive const& lhs, SpherePrimitive const& rhs);

	static bool CheckCollision(PlanePrimitive const& lhs, PointPrimitive const& rhs);
	static bool CheckCollision(PlanePrimitive const& lhs, RayPrimitive const& rhs);
	//static bool CheckCollision(PlanePrimitive const& lhs, TrianglePrimitive const& rhs);
	//static bool CheckCollision(PlanePrimitive const& lhs, PlanePrimitive const& rhs);
	static bool CheckCollision(PlanePrimitive const& lhs, AABBPrimitive const& rhs);
	static bool CheckCollision(PlanePrimitive const& lhs, SpherePrimitive const& rhs);

	static bool CheckCollision(AABBPrimitive const& lhs, PointPrimitive const& rhs);
	static bool CheckCollision(AABBPrimitive const& lhs, RayPrimitive const& rhs);
	//static bool CheckCollision(AABBPrimitive const& lhs, TrianglePrimitive const& rhs);
	static bool CheckCollision(AABBPrimitive const& lhs, PlanePrimitive const& rhs);
	static bool CheckCollision(AABBPrimitive const& lhs, AABBPrimitive const& rhs);
	static bool CheckCollision(AABBPrimitive const& lhs, SpherePrimitive const& rhs);

	static bool CheckCollision(SpherePrimitive const& lhs, PointPrimitive const& rhs);
	static bool CheckCollision(SpherePrimitive const& lhs, RayPrimitive const& rhs);
	//static bool CheckCollision(SpherePrimitive const& lhs, TrianglePrimitive const& rhs);
	static bool CheckCollision(SpherePrimitive const& lhs, PlanePrimitive const& rhs);
	static bool CheckCollision(SpherePrimitive const& lhs, AABBPrimitive const& rhs);
	static bool CheckCollision(SpherePrimitive const& lhs, SpherePrimitive const& rhs);

	// These need to return INT representative of in/out/on
	static int CheckCollision(glm::vec4 const& plane, AABBBV const& bv);
	static int CheckCollision(glm::vec4 const& plane, OBBBV const& bv);
	static int CheckCollision(glm::vec4 const& plane, SphereBV const& bv);

	static bool CheckCollision(RayPrimitive const& ray, FrustumBV const& bv, float* tI = nullptr, float* tO = nullptr);
	static bool CheckCollision(RayPrimitive const& ray, AABBBV const& bv, float* tI = nullptr, float* tO = nullptr);
	static bool CheckCollision(RayPrimitive const& ray, OBBBV const& bv, float* tI = nullptr, float* tO = nullptr);
	static bool CheckCollision(RayPrimitive const& ray, SphereBV const& bv, float* tI = nullptr, float* tO = nullptr);

public: // Helper functions, most of these are directly from the orange textbook
	static bool IntersectPointTriangle(glm::vec3 const& p, glm::vec3 const& q0, glm::vec3 const& q1, glm::vec3 const& q2, glm::vec3 const& qn);
	static bool IntersectSegmentPlane(RayPrimitive const& ray, PlanePrimitive const& plane, PointPrimitive* oPoint = nullptr);
};