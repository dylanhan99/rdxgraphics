#pragma once

struct Intersection
{
	static void CalculateAABBBV(std::vector<glm::vec3> const& positions, glm::vec3& outCenter, glm::vec3& outHalfExtents);

	// -1 >> Outside
	//  0 >> Intersect
	// +1 >> Inside
	static int PlaneSphereTest(glm::vec3 aPos, float aRadius, glm::vec4 bEquation);
};