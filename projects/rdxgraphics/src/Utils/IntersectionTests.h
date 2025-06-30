#pragma once

struct Intersection
{
	static void MostSeparatedPointsOnAABB(std::vector<glm::vec3> const& pt, size_t& oMinI, size_t& oMaxI);
	static void SphereOfSphereAndPt(glm::vec3 const& point, glm::vec3& spherePos, float& radius);
	static void SphereOfSphereAndSphere(glm::vec3 const& s0, float const& r0, glm::vec3 const& s1, float const& r1, glm::vec3& oS, float& oR);
	static void RitterGrowth(void const* points, size_t const length, glm::vec3& spherePos, float& radius);
	static void RitterGrowth(std::vector<glm::vec3> const& points, glm::vec3& spherePos, float& radius);

	static void PCA(std::vector<glm::vec3> const& points, glm::vec3* oCentroid = nullptr, float* oRadius = nullptr, glm::mat3* oRotation = nullptr, glm::vec3* oHalfExtents = nullptr);

	// -1 >> Outside
	//  0 >> Intersect
	static int PointSphereTest(glm::vec3 pointPos, glm::vec3 spherePos, float radius);

	// -1 >> Outside
	//  0 >> Intersect
	// +1 >> Inside
	static int PlaneSphereTest(glm::vec3 aPos, float aRadius, glm::vec4 bEquation);
	static int PlaneAABBTest(glm::vec3 aPos, glm::vec3 aHalfExtents, glm::vec4 bEquation);
	static int PlanePointTest(glm::vec3 aPos, glm::vec4 bEquation);

	// These are unoptimized, no early exit version because it supports tI and tO
	static bool RayAABBTest(
		glm::vec3 const rayPos, glm::vec3 const rayDir,
		glm::vec3 const aabbPos, glm::vec3 const halfExtents,
		float* tI = nullptr, float* tO = nullptr);
	static bool RayOBBTest(
		glm::vec3 const rayPos, glm::vec3 const rayDir,
		glm::vec3 const obbPos, glm::vec3 const halfExtents, glm::mat3 const orthonormalBasis,
		float* tI = nullptr, float* tO = nullptr);
	static bool RaySphereTest(
		glm::vec3 const rayPos, glm::vec3 const rayDir, 
		glm::vec3 const spherePos, float const sphereRadius,
		float* tI = nullptr, float* tO = nullptr);
};