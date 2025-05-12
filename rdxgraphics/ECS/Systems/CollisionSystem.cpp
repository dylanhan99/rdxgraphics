#include <pch.h>
#include "CollisionSystem.h"
#include "ECS/EntityManager.h"
#include "Utils/Input.h"

void CollisionSystem::Update(float dt)
{
	// Hardcoding here to un-set it for the next frame for now
	// Maybe do some colliding pairs system in the future for efficiency if needed
#define _RX_X(Klass){														\
	auto bvView = EntityManager::View<Klass##BV>();\
		for (auto [handle, bv] : bvView.each()) { bv.IsCollide() = false; } \
	}

	RX_DO_ALL_BV_ENUM;
#undef _RX_X

	auto colView = EntityManager::View<Collider>();
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

#define _RX_C_C(RKlass, LKlass)												\
	case BV::RKlass:														\
	{																		\
		LKlass##BV& lbv = EntityManager::GetComponent<LKlass##BV>(lhandle);	\
		RKlass##BV& rbv = EntityManager::GetComponent<RKlass##BV>(rhandle);	\
		if (CheckCollision(lbv, rbv))										\
		{																	\
			lbv.IsCollide() |= true;										\
			rbv.IsCollide() |= true;										\
		}																	\
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
{ // Orange book page 176 (215 in the pdf)
	glm::vec3 d = lhs.GetDirection();
	glm::vec3 n = rhs.GetNormal();
	float dA = glm::dot(d, n);
	if (glm::abs(dA) < glm::epsilon<float>()) // Parallel
		return false;

	float t = (rhs.GetD() - glm::dot(n, lhs.GetPosition())) / dA;
	return t >= 0.f;
}

bool CollisionSystem::CheckCollision(RayBV const& lhs, AABBBV const& rhs)
{ // Orange book page 179 (218 in the pdf)
	glm::vec3 p = lhs.GetPosition();
	glm::vec3 d = lhs.GetDirection();
	glm::vec3 amin = rhs.GetMinPoint();
	glm::vec3 amax = rhs.GetMaxPoint();
	float tmin = 0.f; // set to -FLT_MAX to get first hit on line
	float tmax = FLT_MAX; // set to max distance ray can travel (for segment)

	// For all three slabs
	for (int i = 0; i < 3; i++) 
	{
		if (glm::abs(d[i]) < glm::epsilon<float>()) 
		{
			// Ray is parallel to slab. No hit if origin not within slab
			if (p[i] < amin[i] || p[i] > amax[i]) 
				return false;
		}
		else 
		{
			// Compute intersection t value of ray with near and far plane of slab
			float ood = 1.f / d[i];
			float t1 = (amin[i] - p[i]) * ood;
			float t2 = (amax[i] - p[i]) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2) std::swap(t1, t2);
			// Compute the intersection of slab intersection intervals
			if (t1 > tmin) tmin = t1;
			if (t2 > tmax) tmax = t2;
			// Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax) 
				return false;
		}
	}
	// Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
	//q = p + d * tmin;
	return true;
}

bool CollisionSystem::CheckCollision(RayBV const& lhs, SphereBV const& rhs)
{ // Orange book page 178 (217 in the pdf)
	glm::vec3 m = lhs.GetPosition() - rhs.GetPosition();
	float c = glm::dot(m, m) - glm::pow(rhs.GetRadius(), 2.f);
	// If there is definitely at least one real root, there must be an intersection
	if (c <= 0.f) 
		return true;

	float b = glm::dot(m, lhs.GetDirection());
	// Early exit if ray origin outside sphere and ray pointing away from sphere
	if (b > 0.f) 
		return false;

	float disc = b * b - c;
	// A negative discriminant corresponds to ray missing sphere
	if (disc < 0.f) 
		return false;

	// Now ray must hit sphere
	return true;
}

bool CollisionSystem::CheckCollision(TriangleBV const& lhs, PointBV const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(TriangleBV const& lhs, RayBV const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(PlaneBV const& lhs, PointBV const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(PlaneBV const& lhs, RayBV const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(PlaneBV const& lhs, AABBBV const& rhs)
{ // Orange book page 164 (203 in the pdf)
	glm::vec3 const& e = rhs.GetHalfExtents();
	glm::vec3 const& n = lhs.GetNormal();
	float r = glm::dot(e, glm::abs(n));
	float s = glm::dot(n, rhs.GetPosition()) - lhs.GetD();
	return glm::abs(s) <= r;
}

bool CollisionSystem::CheckCollision(PlaneBV const& lhs, SphereBV const& rhs)
{ // Orange book page 161 (200 in the pdf)
	float dist = glm::dot(rhs.GetPosition(), lhs.GetNormal()) - lhs.GetD();
	return glm::abs(dist) <= rhs.GetRadius();
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
{ // Orange book page 165 (204 in the pdf)
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
