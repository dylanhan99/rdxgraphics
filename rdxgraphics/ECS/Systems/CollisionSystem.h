#pragma once
#include "ECS/EntityManager.h"

class CollisionSystem : public BaseSingleton<CollisionSystem>
{
	RX_SINGLETON_DECLARATION(CollisionSystem);
public:
	static void Update(float dt);

private:
	// Default will come here.
	template <typename T, typename U>
	static bool CheckCollision(T const& lhs, U const& rhs)
	{
		return false;
	}
	// All supported intersection tests follow.

	static bool CheckCollision(PointBV const& lhs, PointBV const& rhs);
	//static bool CheckCollision(PointBV const& lhs, RayBV const& rhs);
	static bool CheckCollision(PointBV const& lhs, TriangleBV const& rhs);
	static bool CheckCollision(PointBV const& lhs, PlaneBV const& rhs);
	static bool CheckCollision(PointBV const& lhs, AABBBV const& rhs);
	static bool CheckCollision(PointBV const& lhs, SphereBV const& rhs);

	//static bool CheckCollision(RayBV const& lhs, PointBV const& rhs);
	//static bool CheckCollision(RayBV const& lhs, RayBV const& rhs);
	static bool CheckCollision(RayBV const& lhs, TriangleBV const& rhs);
	static bool CheckCollision(RayBV const& lhs, PlaneBV const& rhs);
	static bool CheckCollision(RayBV const& lhs, AABBBV const& rhs);
	static bool CheckCollision(RayBV const& lhs, SphereBV const& rhs);

	static bool CheckCollision(TriangleBV const& lhs, PointBV const& rhs);
	static bool CheckCollision(TriangleBV const& lhs, RayBV const& rhs);
	//static bool CheckCollision(TriangleBV const& lhs, TriangleBV const& rhs);
	//static bool CheckCollision(TriangleBV const& lhs, PlaneBV const& rhs);
	//static bool CheckCollision(TriangleBV const& lhs, AABBBV const& rhs);
	//static bool CheckCollision(TriangleBV const& lhs, SphereBV const& rhs);

	static bool CheckCollision(PlaneBV const& lhs, PointBV const& rhs);
	static bool CheckCollision(PlaneBV const& lhs, RayBV const& rhs);
	//static bool CheckCollision(PlaneBV const& lhs, TriangleBV const& rhs);
	//static bool CheckCollision(PlaneBV const& lhs, PlaneBV const& rhs);
	static bool CheckCollision(PlaneBV const& lhs, AABBBV const& rhs);
	static bool CheckCollision(PlaneBV const& lhs, SphereBV const& rhs);

	static bool CheckCollision(AABBBV const& lhs, PointBV const& rhs);
	static bool CheckCollision(AABBBV const& lhs, RayBV const& rhs);
	//static bool CheckCollision(AABBBV const& lhs, TriangleBV const& rhs);
	static bool CheckCollision(AABBBV const& lhs, PlaneBV const& rhs);
	static bool CheckCollision(AABBBV const& lhs, AABBBV const& rhs);
	static bool CheckCollision(AABBBV const& lhs, SphereBV const& rhs);

	static bool CheckCollision(SphereBV const& lhs, PointBV const& rhs);
	static bool CheckCollision(SphereBV const& lhs, RayBV const& rhs);
	//static bool CheckCollision(SphereBV const& lhs, TriangleBV const& rhs);
	static bool CheckCollision(SphereBV const& lhs, PlaneBV const& rhs);
	static bool CheckCollision(SphereBV const& lhs, AABBBV const& rhs);
	static bool CheckCollision(SphereBV const& lhs, SphereBV const& rhs);

public: // Helper functions, most of these are directly from the orange textbook
	static bool IntersectSegmentPlane(RayBV const& ray, PlaneBV const& plane, PointBV* oPoint = nullptr);
};