#include <pch.h>
#include "IntersectionTests.h"

void Intersection::CalculateAABBBV(std::vector<glm::vec3> const& positions, glm::vec3 &outCenter, glm::vec3& outHalfExtents)
{
	glm::vec3 min{std::numeric_limits<float>::max()}, max{ std::numeric_limits<float>::min() };
	for (glm::vec3 const& pos : positions)
	{
		min.x = (pos.x < min.x) ? pos.x : min.x;
		min.y = (pos.y < min.y) ? pos.y : min.y;
		min.z = (pos.z < min.z) ? pos.z : min.z;
	
		max.x = (pos.x > max.x) ? pos.x : max.x;
		max.y = (pos.y > max.y) ? pos.y : max.y;
		max.z = (pos.z > max.z) ? pos.z : max.z;
	}
	
	outCenter = (max + min) * 0.5f;
	outHalfExtents = glm::abs((max - min) * 0.5f);
}
