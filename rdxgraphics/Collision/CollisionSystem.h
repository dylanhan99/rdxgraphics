#pragma once
#include "ECS/EntityManager.h"
#include "Collision/BoundingVolume.h"

class CollisionSystem : public BaseSingleton<CollisionSystem>
{
	RX_SINGLETON_DECLARATION(CollisionSystem);
public:
	static void Update(float dt);

private:
	// Assumes pBV is non-null.
	static bool CheckCollision(Entity::ColliderDetails& lhs, Entity::ColliderDetails& rhs);

	static bool CheckCollision(Point const& lhs, Point const& rhs);
	static bool CheckCollision(Point const& lhs, Ray const& rhs);
	static bool CheckCollision(Point const& lhs, Triangle const& rhs);
	static bool CheckCollision(Point const& lhs, Plane const& rhs);
	static bool CheckCollision(Point const& lhs, AABB const& rhs);
	static bool CheckCollision(Point const& lhs, Sphere const& rhs);

	static bool CheckCollision(Ray const& lhs, Point const& rhs);
	static bool CheckCollision(Ray const& lhs, Ray const& rhs);
	static bool CheckCollision(Ray const& lhs, Triangle const& rhs);
	static bool CheckCollision(Ray const& lhs, Plane const& rhs);
	static bool CheckCollision(Ray const& lhs, AABB const& rhs);
	static bool CheckCollision(Ray const& lhs, Sphere const& rhs);

	static bool CheckCollision(Triangle const& lhs, Point const& rhs);
	static bool CheckCollision(Triangle const& lhs, Ray const& rhs);
	static bool CheckCollision(Triangle const& lhs, Triangle const& rhs);
	static bool CheckCollision(Triangle const& lhs, Plane const& rhs);
	static bool CheckCollision(Triangle const& lhs, AABB const& rhs);
	static bool CheckCollision(Triangle const& lhs, Sphere const& rhs);

	static bool CheckCollision(Plane const& lhs, Point const& rhs);
	static bool CheckCollision(Plane const& lhs, Ray const& rhs);
	static bool CheckCollision(Plane const& lhs, Triangle const& rhs);
	static bool CheckCollision(Plane const& lhs, Plane const& rhs);
	static bool CheckCollision(Plane const& lhs, AABB const& rhs);
	static bool CheckCollision(Plane const& lhs, Sphere const& rhs);

	static bool CheckCollision(AABB const& lhs, Point const& rhs);
	static bool CheckCollision(AABB const& lhs, Ray const& rhs);
	static bool CheckCollision(AABB const& lhs, Triangle const& rhs);
	static bool CheckCollision(AABB const& lhs, Plane const& rhs);
	static bool CheckCollision(AABB const& lhs, AABB const& rhs);
	static bool CheckCollision(AABB const& lhs, Sphere const& rhs);

	static bool CheckCollision(Sphere const& lhs, Point const& rhs);
	static bool CheckCollision(Sphere const& lhs, Ray const& rhs);
	static bool CheckCollision(Sphere const& lhs, Triangle const& rhs);
	static bool CheckCollision(Sphere const& lhs, Plane const& rhs);
	static bool CheckCollision(Sphere const& lhs, AABB const& rhs);
	static bool CheckCollision(Sphere const& lhs, Sphere const& rhs);
};