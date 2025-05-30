#pragma once

struct Intersection
{
	static void CalculateAABBBV(std::vector<glm::vec3> const& positions, glm::vec3& outCenter, glm::vec3& outHalfExtents);
};