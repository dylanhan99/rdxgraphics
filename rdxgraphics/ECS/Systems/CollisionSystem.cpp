#include <pch.h>
#include "CollisionSystem.h"
#include "ECS/EntityManager.h"
#include "Utils/Input.h"

void CollisionSystem::Update(float dt)
{
	// Hardcoding here to un-set it for the next frame for now
	// Maybe do some colliding pairs system in the future for efficiency if needed
#define _RX_X(Klass){														\
	auto bvView = EntityManager::GetInstance().m_Registry.view<Klass##BV>();\
		for (auto [handle, bv] : bvView.each()) { bv.IsCollide() = false; } \
	}

	RX_DO_ALL_BV_ENUM;
#undef _RX_X

	auto colView = EntityManager::GetInstance().m_Registry.view<Collider>();
	for (auto [lhandle, lcol] : colView.each())
	{
		BV lBVType = lcol.GetBVType();
		if (lBVType == BV::NIL)
			continue;
		for (auto [rhandle, rcol] : colView.each())
		{
			if (lhandle == rhandle)
				continue;
			BV rBVType = rcol.GetBVType();
			if (rBVType == BV::NIL)
				continue;

#define _RX_C_C(RKlass, LKlass)																\
	case BV::RKlass:																		\
	{																						\
		LKlass##BV& lbv = EntityManager::GetInstance().m_Registry.get<LKlass##BV>(lhandle); \
		RKlass##BV& rbv = EntityManager::GetInstance().m_Registry.get<RKlass##BV>(rhandle); \
		if (CheckCollision(lbv, rbv))														\
		{																					\
			lbv.IsCollide() |= true;														\
			rbv.IsCollide() |= true;														\
		}																					\
	} break;
#define _RX_C_X(LKlass)							 \
	case BV::LKlass:							 \
	{											 \
		switch (rBVType)						 \
		{										 \
			RX_DO_ALL_BV_ENUM_M_(_RX_C_C, LKlass);\
			default: RX_ASSERT(false); break;    \
		}										 \
	} break;

			switch (lBVType)
			{
				RX_DO_ALL_BV_ENUM_M(_RX_C_X);
			default: RX_ASSERT(false); break;
			}
#undef _RX_C_C
#undef _RX_C_X
		}
	}
}

bool CollisionSystem::CheckCollision(PointBV const& lhs, PointBV const& rhs)
{
	return lhs.GetPosition() == rhs.GetPosition();
}

//bool CollisionSystem::CheckCollision(PointBV const& lhs, RayBV const& rhs)
//{
//	glm::vec3 op = lhs.GetPosition() - rhs.GetPosition();
//	glm::vec3 dir = rhs.GetDirection();
//	float t = glm::dot(op, dir); // Is along the ray direction
//	bool isColinear = glm::length2(glm::cross(op, dir)) <glm::epsilon<float>();
//
//	return (t >= 0.f) && isColinear;
//}

bool CollisionSystem::CheckCollision(PointBV const& lhs, TriangleBV const& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(PointBV const& lhs, PlaneBV const& rhs)
{
	glm::vec3 pp = lhs.GetPosition() - rhs.GetPosition();
	return glm::dot(pp, rhs.GetNormal()) == 0.f;
}

bool CollisionSystem::CheckCollision(PointBV const& lhs, AABBBV const& rhs)
{
	glm::vec3 pos = lhs.GetPosition();
	glm::vec3 min = rhs.GetMinPoint();
	glm::vec3 max = rhs.GetMaxPoint();
	return  (min.x <= pos.x && pos.x <= max.x) &&
			(min.y <= pos.y && pos.y <= max.y) &&
			(min.z <= pos.z && pos.z <= max.z);
}

bool CollisionSystem::CheckCollision(PointBV const& lhs, SphereBV const& rhs)
{
	float d2 = glm::distance2(lhs.GetPosition(), rhs.GetPosition());
	return d2 < glm::pow(rhs.GetRadius(), 2.f);
}

bool CollisionSystem::CheckCollision(RayBV const& lhs, TriangleBV const& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(RayBV const& lhs, PlaneBV const& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(RayBV const& lhs, AABBBV const& rhs)
{
	return false;
}

// NOT CORRECT
// This works for infinite line. 
// But we are doing a directional ray, WITH an point of origin.
// i.e. for a sphere behind point of origin, it should not pass.
bool CollisionSystem::CheckCollision(RayBV const& lhs, SphereBV const& rhs)
{
	glm::vec3 lDir = lhs.GetDirection();
	glm::vec3 L = rhs.GetPosition() - lhs.GetPosition();
	float a = glm::dot(lDir, lDir);
	float b = 2.f * glm::dot(L, lhs.GetDirection());
	float c = glm::dot(L, L) - rhs.GetRadius() * rhs.GetRadius();

	float disc = b * b - 4.f * a * c;

	return disc >= 0.f; // FOr now, we do not care about how many or where the intersections are.
}

bool CollisionSystem::CheckCollision(TriangleBV const& lhs, PointBV const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(TriangleBV const& lhs, RayBV const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(PlaneBV const& lhs, PointBV const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(PlaneBV const& lhs, RayBV const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(PlaneBV const& lhs, AABBBV const& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(PlaneBV const& lhs, SphereBV const& rhs)
{
	return false;
}

bool CollisionSystem::CheckCollision(AABBBV const& lhs, PointBV const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(AABBBV const& lhs, RayBV const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(AABBBV const& lhs, PlaneBV const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(AABBBV const& lhs, AABBBV const& rhs)
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

bool CollisionSystem::CheckCollision(AABBBV const& lhs, SphereBV const& rhs)
{
	// https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection

	// Get the closest point on the sphere to the AABB by clamping
	glm::vec3 boxMin = lhs.GetMinPoint();
	glm::vec3 boxMax = lhs.GetMaxPoint();
	glm::vec3 sphPos = rhs.GetPosition();
	PointBV closestPoint{
		glm::max(boxMin.x, glm::min(sphPos.x, boxMax.x)),
		glm::max(boxMin.y, glm::min(sphPos.y, boxMax.y)),
		glm::max(boxMin.z, glm::min(sphPos.z, boxMax.z))
	};

	return CheckCollision(closestPoint, rhs);
}

bool CollisionSystem::CheckCollision(SphereBV const& lhs, PointBV const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(SphereBV const& lhs, RayBV const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(SphereBV const& lhs, PlaneBV const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(SphereBV const& lhs, AABBBV const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(SphereBV const& lhs, SphereBV const& rhs)
{
	return CheckCollision(
		PointBV{ lhs.GetPosition() },
		SphereBV{ rhs.GetPosition(), lhs.GetRadius() + rhs.GetRadius() }
	);
}
