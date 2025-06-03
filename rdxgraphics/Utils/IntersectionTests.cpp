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

int Intersection::PlaneSphereTest(glm::vec3 aPos, float aRadius, glm::vec4 bEquation)
{
	float dist = glm::dot(aPos, glm::vec3{bEquation}) - bEquation.w;
	if		(dist > aRadius)   return 1;
	else if (dist >= -aRadius) return 0;
	else					   return -1;
}

int Intersection::PlaneAABBTest(glm::vec3 aPos, glm::vec3 aHalfExtents, glm::vec4 bEquation)
{
	glm::vec3 const& e = aHalfExtents;
	glm::vec3 const& n = glm::vec3(bEquation);
	float d = bEquation.w;
	float r = glm::dot(e, glm::abs(n));
	float s = glm::dot(n, glm::vec3(aPos)) - d;
	if		(s > r)   return 1;
	else if (s >= -r) return 0;
	else			  return -1;
}
