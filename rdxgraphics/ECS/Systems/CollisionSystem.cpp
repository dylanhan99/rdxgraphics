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

float ScalarTriple(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	return glm::dot(a, glm::cross(b, c));
}

bool CollisionSystem::CheckCollision(PointBV const& lhs, TriangleBV const& rhs)
{ // Orange book page 204 (243 in the pdf)
	glm::vec3 a = rhs.GetP0();
	glm::vec3 b = rhs.GetP1();
	glm::vec3 c = rhs.GetP2();
	glm::vec3 p = lhs.GetPosition();
	// Translate point and triangle so that point lies at origin
	a -= p; b -= p; c -= p;

	// Plane distance test
	glm::vec3 n = rhs.GetNormal();
	float d = glm::dot(n, a);
	if (glm::abs(d) > glm::epsilon<float>()) return false;

	// Actual test
	float ab = glm::dot(a, b);
	float ac = glm::dot(a, c);
	float bc = glm::dot(b, c);
	float cc = glm::dot(c, c);
	// Make sure plane normals for pab and pbc point in the same direction
	if (bc * ac - cc * ab < 0.0f) return false;
	// Make sure plane normals for pab and pca point in the same direction
	float bb = glm::dot(b, b);
	if (ab * bc - ac * bb < 0.0f) return false;
	return true;
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

// Works for both front and back face
bool CollisionSystem::CheckCollision(RayBV const& lhs, TriangleBV const& rhs)
{ // Orange book page 191 (225 in the pdf)
	glm::vec3 dir = lhs.GetDirection();
	glm::vec3 n = rhs.GetNormal_NotNormalized();

	// Test the ray direction
	{
		float nd = glm::dot(n, dir);
		glm::vec3 rl = lhs.GetPosition() - rhs.GetPosition();
		float nrl = glm::dot(n, rl);

		// 1. Face away + ray behind tri, false
		if (nd < glm::epsilon<float>() && nrl < glm::epsilon<float>())
			return false;
		// 4. Face same + ray infront tri, false
		if (nd > -glm::epsilon<float>() && nrl > -glm::epsilon<float>())
			return false;
	}

	// 2. face away + ray infront tri, test bary
	// 3. Face same + ray behind tri, test bary

	glm::vec3 pa = rhs.GetP0() - lhs.GetPosition();
	glm::vec3 pb = rhs.GetP1() - lhs.GetPosition();
	glm::vec3 pc = rhs.GetP2() - lhs.GetPosition();

	float u = ScalarTriple(dir, pc, pb);
	float v = ScalarTriple(dir, pa, pc);
	float w = ScalarTriple(dir, pb, pa);

	return  (u < 0.f && v < 0.f && w < 0.f) || 
			(u > 0.f && v > 0.f && w > 0.f);
	//if (u < 0.f) return false;
	//if (v < 0.f) return false;
	//if (w < 0.f) return false;
	//
	//return true;
}

bool CollisionSystem::CheckCollision(RayBV const& lhs, PlaneBV const& rhs)
{ // Orange book page 176 (215 in the pdf)
	return IntersectSegmentPlane(lhs, rhs);
}

bool CollisionSystem::CheckCollision(RayBV const& lhs, AABBBV const& rhs)
{ // Orange book page 179 (218 in the pdf)
	glm::vec3 dir = lhs.GetDirection();      // Should be normalized
	glm::vec3 p = lhs.GetPosition();
	glm::vec3 invDir = 1.0f / dir;           // May cause div-by-zero if dir is 0 on any axis

	glm::vec3 t1 = (rhs.GetMinPoint() - p) * invDir;
	glm::vec3 t2 = (rhs.GetMaxPoint() - p) * invDir;

	glm::vec3 tmin = glm::min(t1, t2);
	glm::vec3 tmax = glm::max(t1, t2);

	float tNear = glm::compMax(tmin);
	float tFar = glm::compMin(tmax);

	return tNear <= tFar && tFar >= 0.0f;
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

bool CollisionSystem::IntersectSegmentPlane(RayBV const& ray, PlaneBV const& plane, PointBV* oPoint)
{
	glm::vec3 d = ray.GetDirection();
	glm::vec3 n = plane.GetNormal();
	float dA = glm::dot(d, n);
	if (glm::abs(dA) < glm::epsilon<float>()) // Parallel
		return false;

	float t = (plane.GetD() - glm::dot(n, ray.GetPosition())) / dA;
	if (t >= 0.f)
	{
		if (oPoint) *oPoint = ray.GetPosition() + t * d;
		return true;
	}
	return false;
}
