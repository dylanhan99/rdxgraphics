#include "IntersectionTests.h"

#include <Eigen/Dense>
#include <Eigen/Geometry>

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

void Intersection::PCA(std::vector<glm::vec3> const& points, glm::vec3* oCentroid, float* oRadius, glm::mat3* oRotation, glm::vec3* oHalfExtents)
{
	// x, y, z
	// x, y, z
	// x, y, z
	// ...
	Eigen::MatrixXf pointMatrix{ points.size(), 3 };
	for (size_t i = 0; i < points.size(); ++i)
	{
		auto const& p = points[i];

		pointMatrix(i, 0) = p.x;
		pointMatrix(i, 1) = p.y;
		pointMatrix(i, 2) = p.z;
	}

	Eigen::Vector3f centroid = pointMatrix.colwise().mean();
	Eigen::MatrixXf centered = pointMatrix.rowwise() - centroid.transpose();
	Eigen::Matrix3f covariance = (centered.adjoint() * centered) / (float)points.size();
	Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> es{ covariance };
	Eigen::Matrix3f eigenVectors = es.eigenvectors();
	Eigen::Vector3f eigenValues = es.eigenvalues();
	int maxValIndex = 0;
	if (eigenValues(1) > eigenValues[maxValIndex]) maxValIndex = 1;
	if (eigenValues(2) > eigenValues[maxValIndex]) maxValIndex = 2;

	Eigen::Vector3f principleDirection = eigenVectors.col(maxValIndex);

	// Find the most extreme points along direction 'principleDirection'
	int imin{}, imax{};
	//Intersection::ExtremePointsAlongDirection(principleDirection, pointsCopy, &imin, &imax);
	{
		float minproj{ std::numeric_limits<float>::infinity() };
		float maxproj{ -std::numeric_limits<float>::infinity() };
		for (size_t i = 0; i < points.size(); i++)
		{
			// Project vector from origin to point onto direction vector
			Eigen::Vector3f pt = pointMatrix.row(i);
			float proj = pt.dot(principleDirection);

			// Keep track of least distant point along direction vector
			if (proj < minproj)
			{
				minproj = proj;
				imin = i;
			}
			// Keep track of most distant point along direction vector
			if (proj > maxproj)
			{
				maxproj = proj;
				imax = i;
			}
		}
	}
	//Eigen::Vector3f mid = (pointMatrix.row(imax) + pointMatrix.row(imin)) * 0.5f;
	float dist = (pointMatrix.row(imax) - pointMatrix.row(imin)).norm();

	// Project centered points onto local axes
	Eigen::MatrixXf localPoints = centered * eigenVectors;
	// For each axis, find min/max
	Eigen::Vector3f min = localPoints.colwise().minCoeff();
	Eigen::Vector3f max = localPoints.colwise().maxCoeff();

	if (oCentroid)
	{
		centroid = centroid + eigenVectors * ((min + max) * 0.5f);
		std::memcpy(oCentroid, centroid.data(), sizeof(glm::vec3));
	}
	if (oRadius)
		*oRadius = dist * 0.5f;
	if (oRotation)
		std::memcpy(oRotation, eigenVectors.data(), sizeof(glm::mat3));
	if (oHalfExtents)
	{
		Eigen::Vector3f halfExtents = (max - min) * 0.5f;
		std::memcpy(oHalfExtents, halfExtents.data(), sizeof(glm::vec3));
	}

	// Use ritter to grow
	//pointMatrix.transposeInPlace(); // need to make matrix.data be in col major
	//Intersection::RitterGrowth(pointMatrix.data(), pointMatrix.rows(), oCentroid, oRadius);
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

int Intersection::PlanePointTest(glm::vec3 aPos, glm::vec4 bEquation)
{
	glm::vec3 bNormal = glm::vec3{ bEquation };
	glm::vec3 bPos = bEquation.w * bNormal;

	glm::vec3 ba = aPos - bPos;
	float dist = glm::dot(ba, bNormal);

	// More leeway for floating point error due to my crappy conversions from euler to normal
	constexpr float zero = glm::epsilon<float>() * 10.f;

	if		(dist >  zero) return 1;
	else if (dist > -zero) return 0;
	else				   return -1;
}


bool Intersection::RayAABBTest(glm::vec3 const rayPos, glm::vec3 const rayDir, glm::vec3 const aabbPos, glm::vec3 const halfExtents, float* tI, float* tO)
{
	glm::vec3 invDir = 1.0f / rayDir; // May cause div-by-zero if dir is 0 on any axis
	glm::vec3 min = aabbPos - halfExtents;
	glm::vec3 max = aabbPos + halfExtents;

	glm::vec3 t1 = (min - rayPos) * invDir;
	glm::vec3 t2 = (max - rayPos) * invDir;

	glm::vec3 tmin = glm::min(t1, t2);
	glm::vec3 tmax = glm::max(t1, t2);

	float tNear = glm::compMax(tmin);
	float tFar = glm::compMin(tmax);

	if (tI) *tI = tNear;
	if (tO) *tO = tFar;

	return tNear <= tFar && tFar >= 0.0f;
}

bool Intersection::RayOBBTest(glm::vec3 const rayPos, glm::vec3 const rayDir, glm::vec3 const obbPos, glm::vec3 const halfExtents, glm::mat3 const orthonormalBasis, float* tI, float* tO)
{ // https://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/
	glm::vec3 delta = obbPos - rayPos;
	float tNear{ -std::numeric_limits<float>().infinity() };
	float tFar{ std::numeric_limits<float>().infinity() };
	for (int i = 0; i < 3; ++i)
	{
		glm::vec3 const& axis = orthonormalBasis[i];
		float e = glm::dot(axis, delta);
		float f = glm::dot(rayDir, axis);

		if (glm::abs(f) > 0.f)
		{
			float t1 = (e + halfExtents[i]) / f;
			float t2 = (e - halfExtents[i]) / f;

			if (t1 > t2) std::swap(t1, t2);

			tNear = glm::max(tNear, t1);
			tFar  = glm::min(tFar, t2);

			if (tFar < tNear)
				return false;
		}
		else
		{
			if (std::abs(e) > halfExtents[i])
				return false;
		}
	}

	if (tI) *tI = tNear;
	if (tO) *tO = tFar;

	return true;
}

bool Intersection::RaySphereTest(glm::vec3 const rayPos, glm::vec3 const rayDir, glm::vec3 const spherePos, float const sphereRadius, float* tI, float* tO)
{
	glm::vec3 m = rayPos - spherePos;
	float b = glm::dot(m, rayDir);
	float c = glm::dot(m, m) - glm::dot(sphereRadius, sphereRadius);

	float disc = b * b - c;
	// A negative discriminant corresponds to ray missing sphere
	if (disc < 0.f)
		return false;

	float sqrtDisc = glm::sqrt(disc);
	if (tI) *tI = -b - sqrtDisc;
	if (tO) *tO = -b + sqrtDisc;
	return true;
}
