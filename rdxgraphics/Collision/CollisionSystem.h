#pragma once
#include "Entity/EntityManager.h"
#include "Collision/BoundingVolume.h"

class CollisionSystem : public BaseSingleton<CollisionSystem>
{
	RX_SINGLETON_DECLARATION(CollisionSystem);
public:
	static void Update(float dt);

private:
	// Assumes pBV is non-null.
	static bool CheckCollision(Entity::ColliderDetails& lhs, Entity::ColliderDetails& rhs);

	static bool CheckCollision(Point& lhs, Point& rhs);
	static bool CheckCollision(Point& lhs, Ray& rhs);
	static bool CheckCollision(Point& lhs, Plane& rhs);
	static bool CheckCollision(Point& lhs, AABB& rhs);
	static bool CheckCollision(Point& lhs, Sphere& rhs);

	static bool CheckCollision(Ray& lhs, Point& rhs);
	static bool CheckCollision(Ray& lhs, Ray& rhs);
	static bool CheckCollision(Ray& lhs, Plane& rhs);
	static bool CheckCollision(Ray& lhs, AABB& rhs);
	static bool CheckCollision(Ray& lhs, Sphere& rhs);

	static bool CheckCollision(Plane& lhs, Point& rhs);
	static bool CheckCollision(Plane& lhs, Ray& rhs);
	static bool CheckCollision(Plane& lhs, Plane& rhs);
	static bool CheckCollision(Plane& lhs, AABB& rhs);
	static bool CheckCollision(Plane& lhs, Sphere& rhs);

	static bool CheckCollision(AABB& lhs, Point& rhs);
	static bool CheckCollision(AABB& lhs, Ray& rhs);
	static bool CheckCollision(AABB& lhs, Plane& rhs);
	static bool CheckCollision(AABB& lhs, AABB& rhs);
	static bool CheckCollision(AABB& lhs, Sphere& rhs);

	static bool CheckCollision(Sphere& lhs, Point& rhs);
	static bool CheckCollision(Sphere& lhs, Ray& rhs);
	static bool CheckCollision(Sphere& lhs, Plane& rhs);
	static bool CheckCollision(Sphere& lhs, AABB& rhs);
	static bool CheckCollision(Sphere& lhs, Sphere& rhs);
};