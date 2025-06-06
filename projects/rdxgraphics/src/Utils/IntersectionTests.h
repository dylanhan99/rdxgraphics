#pragma once

struct Intersection
{
	static void MostSeparatedPointsOnAABB(std::vector<glm::vec3> const& pt, size_t& oMinI, size_t& oMaxI);
	static void CalculateAABBBV(std::vector<glm::vec3> const& positions, glm::vec3& outCenter, glm::vec3& outHalfExtents);
	static void SphereOfSphereAndPt(glm::vec3 const& point, glm::vec3& spherePos, float& radius);
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
};