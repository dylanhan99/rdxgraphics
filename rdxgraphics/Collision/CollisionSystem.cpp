#include <pch.h>
#include "CollisionSystem.h"
#include "Utils/Input.h"

void CollisionSystem::Update(float dt)
{
	auto& entities = EntityManager::GetEntities();

	// Hardcoding here to un-set it for the next frame for now
	// Not really sure how else to handle this rn, considering the design choice in the "ECS"
	for (Entity& e : entities)
	{
		if (auto pp = e.GetColliderDetails().pBV)
			pp->IsCollide() = false;
	}

	for (Entity& lhs : entities)
	{
		Entity::ColliderDetails& lhsCol = lhs.GetColliderDetails();
		if (!lhsCol.pBV)
			continue;

		for (Entity& rhs : entities)
		{
			Entity::ColliderDetails& rhsCol = rhs.GetColliderDetails();
			if (!rhsCol.pBV)
				continue;

			if (lhsCol.pBV.get() == rhsCol.pBV.get())
				continue;

			if (CheckCollision(lhsCol, rhsCol))
			{
				lhsCol.pBV->IsCollide() |= true;
				rhsCol.pBV->IsCollide() |= true;
			}
		}
	}
}

bool CollisionSystem::CheckCollision(Entity::ColliderDetails& lhs, Entity::ColliderDetails& rhs)
{
#define _RX_C_C(LKlass, RKlass) case BV::RKlass: return CheckCollision(*(LKlass*)(lhs.pBV.get()), *(RKlass*)(rhs.pBV.get()))
#define _RX_C_X(LKlass)						\
	case BV::LKlass:						\
		switch (rhs.BVType)					\
		{									\
			_RX_C_C(LKlass, Point);			\
			_RX_C_C(LKlass, Ray);			\
			_RX_C_C(LKlass, Plane);			\
			_RX_C_C(LKlass, AABB);			\
			_RX_C_C(LKlass, Sphere);		\
			default: RX_ASSERT(false); break; \
		}

	switch (lhs.BVType)
	{
		_RX_C_X(Point);
		_RX_C_X(Ray);
		_RX_C_X(Plane);
		_RX_C_X(AABB);
		_RX_C_X(Sphere);
	default: RX_ASSERT(false); break;
	}

#undef _RX_C_C
#undef _RX_C_X
}

bool CollisionSystem::CheckCollision(Point& lhs, Point& rhs)
{
	return lhs.GetPosition() == rhs.GetPosition();
}

bool CollisionSystem::CheckCollision(Point& lhs, Ray& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Point& lhs, Plane& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Point& lhs, AABB& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Point& lhs, Sphere& rhs)
{
	float d2 = glm::distance2(lhs.GetPosition(), rhs.GetPosition());
	return d2 < glm::pow(rhs.GetRadius(), 2.f);
}

bool CollisionSystem::CheckCollision(Ray& lhs, Point& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Ray& lhs, Ray& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Ray& lhs, Plane& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Ray& lhs, AABB& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Ray& lhs, Sphere& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Plane& lhs, Point& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Plane& lhs, Ray& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Plane& lhs, Plane& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Plane& lhs, AABB& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Plane& lhs, Sphere& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(AABB& lhs, Point& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(AABB& lhs, Ray& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(AABB& lhs, Plane& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(AABB& lhs, AABB& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(AABB& lhs, Sphere& rhs)
{
	// https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection

	// Get the closest point on the sphere to the AABB by clamping
	glm::vec3 boxMin = lhs.GetMinPoint();
	glm::vec3 boxMax = lhs.GetMaxPoint();
	glm::vec3 sphPos = rhs.GetPosition();
	Point closestPoint{
		glm::fmax(boxMin.x, glm::fmin(sphPos.x, boxMax.x)),
		glm::fmax(boxMin.y, glm::fmin(sphPos.y, boxMax.y)),
		glm::fmax(boxMin.z, glm::fmin(sphPos.z, boxMax.z))
	};

	return CheckCollision(closestPoint, rhs);
}

bool CollisionSystem::CheckCollision(Sphere& lhs, Point& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Sphere& lhs, Ray& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Sphere& lhs, Plane& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Sphere& lhs, AABB& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Sphere& lhs, Sphere& rhs)
{
	return false;
}
