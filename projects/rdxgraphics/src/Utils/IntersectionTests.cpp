#include "IntersectionTests.h"

void Intersection::MostSeparatedPointsOnAABB(std::vector<glm::vec3> const& pt, size_t& oMinI, size_t& oMaxI)
{ // oragne book page 90 (129 in pdf)
	// First find most extreme points along principal axes
	int minx = 0, maxx = 0, miny = 0, maxy = 0, minz = 0, maxz = 0;
	for (size_t i = 1; i < pt.size(); ++i) 
	{
		if (pt[i].x < pt[minx].x) minx = i;
		if (pt[i].x > pt[maxx].x) maxx = i;
		if (pt[i].y < pt[miny].y) miny = i;
		if (pt[i].y > pt[maxy].y) maxy = i;
		if (pt[i].z < pt[minz].z) minz = i;
		if (pt[i].z > pt[maxz].z) maxz = i;
	}

	// Compute the squared distances for the three pairs of points
	float dist2x = glm::dot(pt[maxx] - pt[minx], pt[maxx] - pt[minx]);
	float dist2y = glm::dot(pt[maxy] - pt[miny], pt[maxy] - pt[miny]);
	float dist2z = glm::dot(pt[maxz] - pt[minz], pt[maxz] - pt[minz]);
	// Pick the pair (min,max) of points most distant
	oMinI = minx;
	oMaxI = maxx;
	if (dist2y > dist2x && dist2y > dist2z) 
	{
		oMaxI = maxy;
		oMinI = miny;
	}
	if (dist2z > dist2x && dist2z > dist2y) 
	{
		oMaxI = maxz;
		oMinI = minz;
	}
}

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

void Intersection::SphereOfSphereAndPt(glm::vec3 const& point, glm::vec3& spherePos, float& radius)
{
	glm::vec3 d = point - spherePos;
	float dist2 = glm::dot(d, d);

	if (dist2 > glm::dot(radius, radius))
	{
		float dist = glm::sqrt(dist2);
		float newRadius = (radius + dist) * 0.5f;
		float k = (newRadius - radius) / dist;

		radius = newRadius;
		spherePos += glm::normalize(d) * k;
	}
}

void Intersection::RitterGrowth(void const* points, size_t const length, glm::vec3& spherePos, float& radius)
{
	glm::vec3 const* pData = reinterpret_cast<glm::vec3 const*>(points);
	for (size_t i = 0; i < length; ++i)
	{
		glm::vec3 const& point = pData[i];
		int col = Intersection::PointSphereTest(
			point,
			spherePos, radius
		);

		if (col == -1)
			Intersection::SphereOfSphereAndPt(point, spherePos, radius);
	}
}

void Intersection::RitterGrowth(std::vector<glm::vec3> const& points, glm::vec3& spherePos, float& radius)
{
	RitterGrowth(points.data(), points.size(), spherePos, radius);
}

int Intersection::PointSphereTest(glm::vec3 pointPos, glm::vec3 spherePos, float radius)
{
	float d2 = glm::distance2(pointPos, spherePos);
	return d2 < glm::pow(radius, 2.f) ? 0 : -1;
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
