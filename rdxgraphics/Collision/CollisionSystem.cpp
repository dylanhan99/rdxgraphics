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
#define _RX_C_X(LKlass)							\
	case BV::LKlass:							\
		switch (rhs.BVType)						\
		{										\
			_RX_C_C(LKlass, Point);				\
			_RX_C_C(LKlass, Ray);				\
			_RX_C_C(LKlass, Triangle);			\
			_RX_C_C(LKlass, Plane);				\
			_RX_C_C(LKlass, AABB);				\
			_RX_C_C(LKlass, Sphere);			\
			default: RX_ASSERT(false); break;   \
		}

	switch (lhs.BVType)
	{
		_RX_C_X(Point);
		_RX_C_X(Ray);
		_RX_C_X(Triangle);
		_RX_C_X(Plane);
		_RX_C_X(AABB);
		_RX_C_X(Sphere);
	default: RX_ASSERT(false); break;
	}

#undef _RX_C_C
#undef _RX_C_X
}

bool CollisionSystem::CheckCollision(Point const& lhs, Point const& rhs)
{
	return lhs.GetPosition() == rhs.GetPosition();
}

bool CollisionSystem::CheckCollision(Point const& lhs, Ray const& rhs)
{
	glm::vec3 op = lhs.GetPosition() - rhs.GetPosition();
	glm::vec3 dir = rhs.GetDirection();
	float t = glm::dot(op, dir); // Is along the ray direction
	bool isColinear = glm::length2(glm::cross(op, dir)) < glm::epsilon<float>();

	return (t >= 0.f) && isColinear;
}

bool CollisionSystem::CheckCollision(Point const& lhs, Triangle const& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Point const& lhs, Plane const& rhs)
{
	glm::vec3 pp = lhs.GetPosition() - rhs.GetPosition();
	return glm::dot(pp, rhs.GetNormal()) == 0.f;
}

bool CollisionSystem::CheckCollision(Point const& lhs, AABB const& rhs)
{
	glm::vec3 pos = lhs.GetPosition();
	glm::vec3 min = rhs.GetMinPoint();
	glm::vec3 max = rhs.GetMaxPoint();
	return  (min.x <= pos.x && pos.x <= max.x) &&
			(min.y <= pos.y && pos.y <= max.y) &&
			(min.z <= pos.z && pos.z <= max.z);
}

bool CollisionSystem::CheckCollision(Point const& lhs, Sphere const& rhs)
{
	float d2 = glm::distance2(lhs.GetPosition(), rhs.GetPosition());
	return d2 < glm::pow(rhs.GetRadius(), 2.f);
}

bool CollisionSystem::CheckCollision(Ray const& lhs, Point const& rhs) { return CheckCollision(rhs, lhs); }

// NOT WORKING
bool CollisionSystem::CheckCollision(Ray const& lhs, Ray const& rhs)
{
	// 1. Coplanar
	// 2. Both directions do intersect

	glm::vec3 p1 = lhs.GetPosition();
	glm::vec3 p2 = rhs.GetPosition();
	glm::vec3 d1 = glm::normalize(lhs.GetDirection());
	glm::vec3 d2 = glm::normalize(rhs.GetDirection());

	// Colinear check
	glm::vec3 r = p1 - p2;
	glm::vec3 crossDir = glm::cross(d1, d2);
	float eps2 = glm::pow(glm::epsilon<float>(), 2.f);
	if (glm::length2(crossDir) < eps2)
	{ // ||crossDir|| == 0.f if the (normalized) the directional vectors are parallel
		glm::vec3 crossR = glm::cross(r, d1);
		return glm::length2(crossR) < eps2;
	}

	// Not parallel, must check minimum distance between both rays.
	// Not parallel — check minimal distance
	float a = glm::dot(d1, d1);
	float b = glm::dot(d1, d2);
	float c = glm::dot(d2, d2);
	float d = glm::dot(d1, r);
	float e = glm::dot(d2, r);

	float denom = a * c - b * b;
	float t1 = (b * e - c * d) / denom;
	float t2 = (a * e - b * d) / denom;

	glm::vec3 j = p1 + t1 * d1;
	glm::vec3 k = p2 + t2 * d2;

	return glm::length2(j - k) < eps2;
}

bool CollisionSystem::CheckCollision(Ray const& lhs, Triangle const& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Ray const& lhs, Plane const& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Ray const& lhs, AABB const& rhs)
{
	return false;
}

// NOT CORRECT
// This works for infinite line. 
// But we are doing a directional ray, WITH an point of origin.
// i.e. for a sphere behind point of origin, it should not pass.
bool CollisionSystem::CheckCollision(Ray const& lhs, Sphere const& rhs)
{
	glm::vec3 lDir = lhs.GetDirection();
	glm::vec3 L = rhs.GetPosition() - lhs.GetPosition();
	float a = glm::dot(lDir, lDir);
	float b = 2.f * glm::dot(L, lhs.GetDirection());
	float c = glm::dot(L, L) - rhs.GetRadius() * rhs.GetRadius();

	float disc = b * b - 4.f * a * c;

	return disc >= 0.f; // FOr now, we do not care about how many or where the intersections are.
}

bool CollisionSystem::CheckCollision(Triangle const& lhs, Point const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(Triangle const& lhs, Ray const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(Triangle const& lhs, Triangle const& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Triangle const& lhs, Plane const& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Triangle const& lhs, AABB const& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Triangle const& lhs, Sphere const& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Plane const& lhs, Point const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(Plane const& lhs, Ray const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(Plane const& lhs, Triangle const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(Plane const& lhs, Plane const& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Plane const& lhs, AABB const& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(Plane const& lhs, Sphere const& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(AABB const& lhs, Point const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(AABB const& lhs, Ray const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(AABB const& lhs, Triangle const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(AABB const& lhs, Plane const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(AABB const& lhs, AABB const& rhs)
{
	glm::vec3 minL{ lhs.GetMinPoint() }, maxL{ lhs.GetMaxPoint() };
	glm::vec3 minR{ rhs.GetMinPoint() }, maxR{ rhs.GetMaxPoint() };

	return
		minL.x <= maxR.x &&
		maxL.x >= minR.x &&
		minL.y <= maxR.y &&
		maxL.y >= minR.y &&
		minL.z <= maxR.z &&
		maxL.z >= minR.z;
}

bool CollisionSystem::CheckCollision(AABB const& lhs, Sphere const& rhs)
{
	// https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection

	// Get the closest point on the sphere to the AABB by clamping
	glm::vec3 boxMin = lhs.GetMinPoint();
	glm::vec3 boxMax = lhs.GetMaxPoint();
	glm::vec3 sphPos = rhs.GetPosition();
	Point closestPoint{
		glm::max(boxMin.x, glm::min(sphPos.x, boxMax.x)),
		glm::max(boxMin.y, glm::min(sphPos.y, boxMax.y)),
		glm::max(boxMin.z, glm::min(sphPos.z, boxMax.z))
	};

	return CheckCollision(closestPoint, rhs);
}

bool CollisionSystem::CheckCollision(Sphere const& lhs, Point const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(Sphere const& lhs, Ray const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(Sphere const& lhs, Triangle const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(Sphere const& lhs, Plane const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(Sphere const& lhs, AABB const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(Sphere const& lhs, Sphere const& rhs)
{
	return CheckCollision(
		Point{ lhs.GetPosition() },
		Sphere{ rhs.GetPosition(), lhs.GetRadius() + rhs.GetRadius() }
	);
}
