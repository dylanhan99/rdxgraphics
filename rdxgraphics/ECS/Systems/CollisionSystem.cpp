#include <pch.h>
#include "CollisionSystem.h"
#include "ECS/EntityManager.h"
#include "Utils/Input.h"
#include "ECS/Components/BoundingVolume.h"
#include "ECS/Systems/RenderSystem.h"
#include "Utils/IntersectionTests.h"

void CollisionSystem::Update(float dt)
{
	// Frustum X other checks
	{
		// This is assuming theres an active camera. Dangerous
		FrustumBV& cameraFrustum = EntityManager::GetComponent<FrustumBV>(RenderSystem::GetActiveCamera());
		auto const& planeEquations = cameraFrustum.GetPlaneEquations();

#define _RX_X(Klass) 												   \
	case BV::Klass: 												   \
	{																   \
		Klass##BV& bv = EntityManager::GetComponent<Klass##BV>(handle);\
		int res{};													   \
		for (glm::vec4 const& plane : planeEquations)				   \
		{															   \
			res = CheckCollision(plane, bv);						   \
			if (res <= 0) break;									   \
		}															   \
		if		(res > 0) bv.SetBVState(BVState::In);				   \
		else if (res < 0) bv.SetBVState(BVState::Out);				   \
		else			  bv.SetBVState(BVState::On);				   \
		break;														   \
	}
		auto bvView = EntityManager::View<BoundingVolume>();
		for (auto [handle, boundingVolume] : bvView.each())
		{
			switch (boundingVolume.GetBVType())
			{
				RX_DO_ALL_BV_ENUM;
			default:
				break;
			}
		}
#undef _RX_X

		// Hardcoding here to un-set it for the next frame for now
		// Maybe do some colliding pairs system in the future for efficiency if needed
#define _RX_X(Klass){															  \
		auto primView = EntityManager::View<Klass##Primitive>();				  \
		for (auto [handle, prim] : primView.each()) { prim.IsCollide() = false; } \
	}

		RX_DO_ALL_PRIMITIVE_ENUM;
#undef _RX_X

		auto colView = EntityManager::View<Collider>();
		for (auto [lhandle, lcol] : colView.each())
		{
			Primitive lPType = lcol.GetPrimitiveType();
			if (lPType == Primitive::NIL)
				continue;
			for (auto [rhandle, rcol] : colView.each())
			{
				if (lhandle == rhandle)
					continue;
				Primitive rPType = rcol.GetPrimitiveType();
				if (rPType == Primitive::NIL)
					continue;

#define _RX_C_C(RKlass, LKlass)					  \
	case Primitive::RKlass:						  \
	{											  \
		LKlass##Primitive& lP = EntityManager::GetComponent<LKlass##Primitive>(lhandle); \
		RKlass##Primitive& rP = EntityManager::GetComponent<RKlass##Primitive>(rhandle); \
		if (CheckCollision(lP, rP))				  \
		{										  \
			lP.IsCollide() |= true;				  \
			rP.IsCollide() |= true;				  \
		}										  \
	} break;
#define _RX_C_X(LKlass)							   \
	case Primitive::LKlass:						   \
	{											   \
		switch (rPType)							   \
		{										   \
			RX_DO_ALL_PRIMITIVE_ENUM_M_(_RX_C_C, LKlass); \
			default: RX_ASSERT(false); break;      \
		}										   \
	} break;

			switch (lPType)
			{
				RX_DO_ALL_PRIMITIVE_ENUM_M(_RX_C_X);
			default: RX_ASSERT(false); break;
			}
#undef _RX_C_C
#undef _RX_C_X
			}
		}
	}
}

bool CollisionSystem::CheckCollision(PointPrimitive const& lhs, PointPrimitive const& rhs)
{
	return lhs.GetPosition() == rhs.GetPosition();
}

bool CollisionSystem::CheckCollision(PointPrimitive const& lhs, TrianglePrimitive const& rhs)
{ // Orange book page 204 (243 in the pdf)
	return IntersectPointTriangle(lhs.GetPosition(), rhs.GetP0_W(), rhs.GetP1_W(), rhs.GetP2_W(), rhs.GetNormal());
}

bool CollisionSystem::CheckCollision(PointPrimitive const& lhs, PlanePrimitive const& rhs)
{
	glm::vec3 pp = lhs.GetPosition() - rhs.GetPosition();
	return glm::abs(glm::dot(pp, rhs.GetNormal())) <= (glm::epsilon<float>() * 10.f); 
	// More leeway for floating point error due to my crappy conversions from euler to normal
}

bool CollisionSystem::CheckCollision(PointPrimitive const& lhs, AABBPrimitive const& rhs)
{
	glm::vec3 pos = lhs.GetPosition();
	glm::vec3 min = rhs.GetMinPoint();
	glm::vec3 max = rhs.GetMaxPoint();
	return  (min.x <= pos.x && pos.x <= max.x) &&
			(min.y <= pos.y && pos.y <= max.y) &&
			(min.z <= pos.z && pos.z <= max.z);
}

bool CollisionSystem::CheckCollision(PointPrimitive const& lhs, SpherePrimitive const& rhs)
{
	return IntersectPointSphere(lhs.GetPosition(), rhs.GetPosition(), rhs.GetRadius());
}

// Works for both front and back face
bool CollisionSystem::CheckCollision(RayPrimitive const& lhs, TrianglePrimitive const& rhs)
{ // Orange book page 191 (225 in the pdf)
	static auto ScalarTriple = 
		[](glm::vec3 a, glm::vec3 b, glm::vec3 c)
		{
			return glm::dot(a, glm::cross(b, c));
		};

	if (IntersectPointTriangle(lhs.GetPosition(), rhs.GetP0_W(), rhs.GetP1_W(), rhs.GetP2_W(), rhs.GetNormal()))
		return true;

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

	glm::vec3 pa = rhs.GetP0_W() - lhs.GetPosition();
	glm::vec3 pb = rhs.GetP1_W() - lhs.GetPosition();
	glm::vec3 pc = rhs.GetP2_W() - lhs.GetPosition();

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

bool CollisionSystem::CheckCollision(RayPrimitive const& lhs, PlanePrimitive const& rhs)
{ // Orange book page 176 (215 in the pdf)
	return IntersectSegmentPlane(lhs, rhs);
}

bool CollisionSystem::CheckCollision(RayPrimitive const& lhs, AABBPrimitive const& rhs)
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

bool CollisionSystem::CheckCollision(RayPrimitive const& lhs, SpherePrimitive const& rhs)
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

bool CollisionSystem::CheckCollision(TrianglePrimitive const& lhs, PointPrimitive const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(TrianglePrimitive const& lhs, RayPrimitive const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(PlanePrimitive const& lhs, PointPrimitive const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(PlanePrimitive const& lhs, RayPrimitive const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(PlanePrimitive const& lhs, AABBPrimitive const& rhs)
{ // Orange book page 164 (203 in the pdf)
	return Intersection::PlaneAABBTest(rhs.GetPosition(), rhs.GetHalfExtents(), glm::vec4{ lhs.GetNormal(), lhs.GetD() }) == 0;
}

bool CollisionSystem::CheckCollision(PlanePrimitive const& lhs, SpherePrimitive const& rhs)
{ // Orange book page 161 (200 in the pdf)
	return Intersection::PlaneSphereTest(rhs.GetPosition(), rhs.GetRadius(), glm::vec4{lhs.GetNormal(), lhs.GetD()}) == 0;
}

bool CollisionSystem::CheckCollision(AABBPrimitive const& lhs, PointPrimitive const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(AABBPrimitive const& lhs, RayPrimitive const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(AABBPrimitive const& lhs, PlanePrimitive const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(AABBPrimitive const& lhs, AABBPrimitive const& rhs)
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

bool CollisionSystem::CheckCollision(AABBPrimitive const& lhs, SpherePrimitive const& rhs)
{ // Orange book page 165 (204 in the pdf)
	// https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
	// Get the closest point on the sphere to the AABB by clamping
	glm::vec3 boxMin = lhs.GetMinPoint();
	glm::vec3 boxMax = lhs.GetMaxPoint();
	glm::vec3 sphPos = rhs.GetPosition();

	glm::vec3 closestPoint{
		glm::max(boxMin.x, glm::min(sphPos.x, boxMax.x)),
		glm::max(boxMin.y, glm::min(sphPos.y, boxMax.y)),
		glm::max(boxMin.z, glm::min(sphPos.z, boxMax.z))
	};

	return IntersectPointSphere(closestPoint, sphPos, rhs.GetRadius());
}

bool CollisionSystem::CheckCollision(SpherePrimitive const& lhs, PointPrimitive const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(SpherePrimitive const& lhs, RayPrimitive const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(SpherePrimitive const& lhs, PlanePrimitive const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(SpherePrimitive const& lhs, AABBPrimitive const& rhs) { return CheckCollision(rhs, lhs); }

bool CollisionSystem::CheckCollision(SpherePrimitive const& lhs, SpherePrimitive const& rhs)
{
	return IntersectPointSphere(
		lhs.GetPosition(),
		rhs.GetPosition(), lhs.GetRadius() + rhs.GetRadius());
}

bool CollisionSystem::CheckCollision(glm::vec4 const& plane, AABBBV const& bv)
{
	return Intersection::PlaneAABBTest(bv.GetPosition(), bv.GetHalfExtents(), plane);
}

bool CollisionSystem::CheckCollision(glm::vec4 const& plane, OBBBV const& bv)
{
	return false;
}

bool CollisionSystem::CheckCollision(glm::vec4 const& plane, SphereBV const& bv)
{
	return Intersection::PlaneSphereTest(bv.GetPosition(), bv.GetRadius(), plane);
}

bool CollisionSystem::IntersectPointTriangle(glm::vec3 const& p, glm::vec3 const& q0, glm::vec3 const& q1, glm::vec3 const& q2, glm::vec3 const& qn)
{
	glm::vec3 a = q0;
	glm::vec3 b = q1;
	glm::vec3 c = q2;
	// Translate point and triangle so that point lies at origin
	a -= p; b -= p; c -= p;

	// Plane distance test
	glm::vec3 n = qn;
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

bool CollisionSystem::IntersectPointSphere(glm::vec3 const& p, glm::vec3 const& q, float radius)
{
	float d2 = glm::distance2(p, q);
	return d2 < glm::pow(radius, 2.f);
}

bool CollisionSystem::IntersectSegmentPlane(RayPrimitive const& ray, PlanePrimitive const& plane, PointPrimitive* oPoint)
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
