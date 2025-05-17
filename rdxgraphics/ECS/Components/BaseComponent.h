#pragma once
#include <entt/entt.hpp>

#define RX_COMPONENT_HAS_HANDLE(Klass) 						 \
public:														 \
inline static const bool HasEnttHandle{ true };				 \
															 \
public:														 \
	Klass() = delete; /*We want to force the entt::entity*/	 \
	inline Klass(entt::entity handle) : m_Handle(handle) {}; \
	inline entt::entity GetEntityHandle() const { return m_Handle; } \
private: entt::entity m_Handle{};

// These cover all "main" components. 
// In the context of collider, Collider is the "main", while _BV variants are the "subsidiaries"
#define RX_DO_MAIN_COMPONENTS_M(F_O_O, ...)\
	F_O_O(Xform, ##__VA_ARGS__);		   \
	F_O_O(Camera, ##__VA_ARGS__);		   \
	F_O_O(Model, ##__VA_ARGS__);		   \
	F_O_O(DirectionalLight, ##__VA_ARGS__);\
	F_O_O(Material, ##__VA_ARGS__);		   \
	F_O_O(Collider, ##__VA_ARGS__);

#define RX_DO_MAIN_COMPONENTS RX_DO_MAIN_COMPONENTS_M(_RX_X)

class BaseComponent
{
public:
	~BaseComponent() = default;
private:

};

class Xform : public BaseComponent
{
public:
	Xform() = default;
	inline Xform(glm::vec3 const& pos, glm::vec3 scale = glm::vec3{1.f}, glm::vec3 eulOri = glm::vec3{0.f})
		: m_Translate(pos), m_Scale(scale), m_Rotate(eulOri) { }

	void UpdateXform();

	inline glm::mat4 const& GetXform() const { return m_Xform; }
	inline glm::mat4& GetXform() { return m_Xform; }
	inline glm::vec3 const& GetTranslate() const { return m_Translate; }
	inline glm::vec3& GetTranslate() { return m_Translate; }
	inline glm::vec3 const& GetScale() const { return m_Scale; }
	inline glm::vec3& GetScale() { return m_Scale; }
	inline glm::vec3 const& GetEulerOrientation() const { return m_Rotate; }
	inline glm::vec3& GetEulerOrientation() { return m_Rotate; }

private:
	glm::vec3 m_Translate{ 0.f }, m_Scale{ 1.f }, m_Rotate{ 0.f };
	glm::mat4 m_Xform{};
};

class Model : public BaseComponent
{
public:
	Model() = default;
	inline Model(Rxuid uid) : m_MeshID(uid) {}
	inline Model(Shape s) : m_MeshID(Rxuid{ s }) {}

	inline Rxuid GetMesh() const { return m_MeshID; }
	inline void SetMesh(Rxuid uid) { m_MeshID = uid; }

private:
	Rxuid m_MeshID{ Rxuid{ Shape::NIL } };
};

class DirectionalLight : public BaseComponent
{
public:
	DirectionalLight() = default;
	inline DirectionalLight(glm::vec3 direction) : m_Direction(direction) {}

	inline glm::vec3 const& GetDirection() const { return m_Direction; }
	inline glm::vec3& GetDirection() { return m_Direction; }

private:
	glm::vec3 m_Direction{}; // Normalized directional vector
	glm::vec3 m_Color{ 1.f };
};

class Material : public BaseComponent
{
public:
	//inline static const uint32_t MaxMaterials{ 100 }; // Default value for uniform size. Must align with expected size in shader.

public:
	Material() = default;
	inline Material(glm::vec3 ambientColor) : m_AmbientColor(ambientColor) {}

	inline glm::vec3 const& GetAmbientColor() const { return m_AmbientColor; }
	inline glm::vec3& GetAmbientColor() { return m_AmbientColor; }
	inline float const& GetAmbientIntensity() const { return m_AmbientIntensity; }
	inline float& GetAmbientIntensity() { return m_AmbientIntensity; }

	inline glm::vec3 const& GetDiffuseColor() const { return m_DiffuseColor; }
	inline glm::vec3& GetDiffuseColor() { return m_DiffuseColor; }
	inline float const& GetDiffuseIntensity() const { return m_DiffuseIntensity; }
	inline float& GetDiffuseIntensity() { return m_DiffuseIntensity; }

	inline glm::vec3 const& GetSpecularColor() const { return m_SpecularColor; }
	inline glm::vec3& GetSpecularColor() { return m_SpecularColor; }
	inline float const& GetSpecularIntensity() const { return m_SpecularIntensity; }
	inline float& GetSpecularIntensity() { return m_SpecularIntensity; }

	inline float const& GetShininess() const { return m_Shininess; }
	inline float& GetShininess() { return m_Shininess; }

	inline operator glm::mat4() const
	{
		return glm::mat4{
			glm::vec4 {m_AmbientColor, m_AmbientIntensity},
			glm::vec4 {m_DiffuseColor, m_DiffuseIntensity},
			glm::vec4 {m_SpecularColor, m_SpecularIntensity},
			glm::vec4 {m_Shininess, 0.f,0.f,0.f} // Padding used by UBO's std140 formatting
		};
	}

private:
	glm::vec3 m_AmbientColor{ 0.f };
	float m_AmbientIntensity{ 0.f };
	glm::vec3 m_DiffuseColor{ 0.f };
	float m_DiffuseIntensity{ 0.f };
	glm::vec3 m_SpecularColor{ 0.f };
	float m_SpecularIntensity{ 0.f };
	float m_Shininess{ 0.f };
	// glm::vec3 padding{}; on shader end (std140)
};

class Collider : public BaseComponent
{
	RX_COMPONENT_HAS_HANDLE(Collider);
public:
	//template <typename T, typename ...Args>
	//inline Collider(Args&& ...args) { SetBV<T>(args...); }
	inline Collider(entt::entity handle, BV bvType) : Collider(handle) { SetBV(bvType); }

	inline BV GetBVType() const { return m_BVType; }

	//template <typename T, typename ...Args>
	//void SetBV(Args&& ...args)
	//{
	//	if (GetBVType() != BV::NIL)
	//	{
	//		// Remove the old one,
	//	}
	//
	//	m_BVType = GetBVType<T>();
	//}
	void SetBV(BV bvType);
	void RemoveBV();

private:
	BV m_BVType{ BV::NIL };
};

class BaseBV : public BaseComponent
{
public:
	BaseBV() = default;
	~BaseBV() = default;
	inline BaseBV(glm::vec3 const& p) : m_Position(p) {}
	inline BaseBV(float x, float y, float z) : m_Position({ x, y, z }) {}

	virtual void UpdateXform() = 0;

	inline glm::mat4 const& GetXform() const { return m_Xform; }
	inline glm::vec3 const& GetPosition() const { return m_Position; }
	inline glm::vec3& GetPosition() { return m_Position; }
	inline bool const& IsCollide() const { return m_IsCollide; }
	inline bool& IsCollide() { return m_IsCollide; }

protected:
	glm::mat4 m_Xform{};
	glm::vec3 m_Position{};
	bool m_IsCollide{ false };
};

class PointBV : public BaseBV
{
public:
	PointBV() = default;
	inline PointBV(glm::vec3 const& p) : BaseBV(p) {}
	inline PointBV(float x, float y, float z) : BaseBV(x, y, z) {}

	inline void UpdateXform() override
	{
		m_Xform = glm::translate(m_Position);
	}
};

class RayBV : public BaseBV
{
public:
	inline static const glm::vec3 DefaultDirection{ 0.f,0.f,-1.f };
public:
	RayBV() = default;
	inline void UpdateXform() override
	{
		//RX_INFO("{} > {} > {}", GetDirection().x, GetDirection().y, GetDirection().z);
		m_Xform = glm::translate(m_Position) * glm::scale(glm::vec3{ s_Scale }) *
			glm::mat4_cast(GetOrientationQuat());
	}

	inline glm::vec3 const& GetOrientation() const { return m_EulerOrientation; }
	inline glm::vec3& GetOrientation() { return m_EulerOrientation; }
	inline glm::quat GetOrientationQuat() const { return std::move(glm::quat{ m_EulerOrientation }); }

	// inward facing is the agreed upon standard for ray
	inline glm::vec3 GetDirection() const { return GetDirection(glm::quat{ m_EulerOrientation }); }

public:
	inline static glm::vec3 GetDirection(glm::quat const& quat) { return quat * DefaultDirection; }

private:
	glm::vec3 m_EulerOrientation{ 0.f,0.f,0.f }; // (Radians) Pitch, Yaw, Roll

	inline static float s_Scale{ 10.f };
};

// Must be CCW orientation
class TriangleBV : public BaseBV
{
public:
	inline static const glm::vec3 DefaultP0{ 0.0f,		 1.0f, 0.f};
	inline static const glm::vec3 DefaultP1{-0.86603f,	-0.5f, 0.f};
	inline static const glm::vec3 DefaultP2{ 0.86603f,	-0.5f, 0.f};
	inline static const glm::vec3 DefaultNormal{ glm::cross(DefaultP1 - DefaultP0, DefaultP2 - DefaultP0) };
public:
	TriangleBV() = default;
	inline void UpdateXform() override
	{
		m_Xform = glm::translate(GetPosition());
		// Set up the linear system Ax = b
	// We'll solve for 12 unknowns (the elements of the transformation matrix)

	// For each corresponding pair of vertices, we get 3 equations
	// We need at least 4 point correspondences to solve for the full affine matrix
	// Since we only have 3 vertices, we'll use the centroid as the 4th point

	// Calculate centroids
		//glm::vec3 centroidA{ 0.f };// (verticesA[0] + verticesA[1] + verticesA[2]) / 3.0f;
		//glm::vec3 centroidB = m_Position;// = (verticesB[0] + verticesB[1] + verticesB[2]) / 3.0f;
		//
		//// Create arrays including the centroids
		//std::array<glm::vec3, 4> pointsA = { DefaultP0, DefaultP1, DefaultP2, centroidA };
		//std::array<glm::vec3, 4> pointsB = { GetP0_W(), GetP1_W(), GetP2_W(), centroidB };

		// For debugging
		//std::cout << "Points in Triangle A (including centroid):\n";
		//for (const auto& p : pointsA) {
		//	std::cout << "  " << glm::to_string(p) << "\n";
		//}
		//
		//std::cout << "Points in Triangle B (including centroid):\n";
		//for (const auto& p : pointsB) {
		//	std::cout << "  " << glm::to_string(p) << "\n";
		//}

		// Method 1: Direct solution using matrix operations
		// We'll set up the equation system in the form PA * M = PB
		// Where PA and PB are matrices containing the point coordinates

		// Create matrices for the linear system
		//glm::mat2 PA{ pointsA[0], pointsA[1] }; // 4 points, 3 coordinates each
		//glm::mat2x3 PB{ pointsB[0], pointsB[1] };
		//
		//glm::mat2x3 M = PB * glm::inverse(PA);
		//
		//glm::vec3 xAxis = M[0]; // transformed d0
		//glm::vec3 yAxis = M[1]; // transformed d1
		//glm::vec3 zAxis = glm::normalize(glm::cross(xAxis, yAxis)); // inferred normal
		//glm::mat4 model = glm::mat4(
		//	glm::vec4(xAxis, 0.f),
		//	glm::vec4(yAxis, 0.f),
		//	glm::vec4(zAxis, 0.f),
		//	glm::vec4(m_Position, 1.f)
		//);
		//m_Xform = model;
		//return;

		//// Fill the matrices with the point coordinates
		//for (int i = 0; i < 4; i++) {
		//	PA[i] = glm::vec3(pointsA[i].x, pointsA[i].y, pointsA[i].z);
		//	PB[i] = glm::vec3(pointsB[i].x, pointsB[i].y, pointsB[i].z);
		//}
		//
		//// Augment PA to include the homogeneous coordinate (1)
		//glm::mat4 A;
		//for (int i = 0; i < 4; i++) {
		//	A[i] = glm::vec4(pointsA[i], 1.0f);
		//}
		//
		//// Calculate the inverse of A
		//glm::mat4 Ainv = glm::inverse(A);
		//
		//// Check if the matrix is invertible
		//if (glm::determinant(A) == 0.0f) {
		//	std::cerr << "Error: Source points are coplanar, transformation cannot be uniquely determined.\n";
		//	m_Xform = glm::mat4(1.0f); // Return identity matrix as fallback
		//	return;
		//}
		//
		//// Solve for the transformation matrix M
		//glm::mat4 M;
		//for (int i = 0; i < 3; i++) {
		//	// Extract column i from PB
		//	glm::vec4 bCol(PB[0][i], PB[1][i], PB[2][i], PB[3][i]);
		//
		//	// Solve for column i of the transformation matrix
		//	glm::vec4 xCol = Ainv * bCol;
		//
		//	// Set the column in the result matrix
		//	M[0][i] = xCol[0];
		//	M[1][i] = xCol[1];
		//	M[2][i] = xCol[2];
		//	M[3][i] = xCol[3];
		//}
		//
		//// Set the last column of the transformation matrix
		//M[0][3] = 0.0f;
		//M[1][3] = 0.0f;
		//M[2][3] = 0.0f;
		//M[3][3] = 1.0f;
		//
		//m_Xform =  M;
	}
	//{
	//	// We have our reference "default" triangle, using DefaultPx
	//	// We can use this against our "deformed" triangle, px_w
	//	//glm::mat3 defaultBasis{ DefaultP0, DefaultP1, DefaultP2 };
	//	glm::mat3 deformed{ m_Position + m_P0, m_Position + m_P1, m_Position + m_P2 };
	//
	//	//// This best-fit matrix is the combination of scale and rotate
	//	//glm::mat3 sclXrot = deformed * glm::inverse(defaultBasis);
	//
	//	glm::vec3 ref_origin{ 0.f };
	//	glm::vec3 ref_x = glm::normalize(DefaultP1 - DefaultP0);
	//	glm::vec3 ref_y = glm::normalize(DefaultP2 - DefaultP0);
	//	glm::vec3 ref_z = glm::normalize(glm::cross(ref_x, ref_y));
	//	glm::mat3 ref_basis = glm::mat3(ref_x, ref_y, ref_z);
	//
	//	glm::vec3 def_origin = m_Position;
	//	glm::vec3 def_x = glm::normalize(deformed[1] - deformed[0]);
	//	glm::vec3 def_y = glm::normalize(deformed[2] - deformed[0]);
	//	glm::vec3 def_z = glm::normalize(glm::cross(def_x, def_y));
	//	glm::mat3 def_basis = glm::mat3(def_x, def_y, def_z);
	//
	//	// scale
	//	auto avg_edge_len = [](const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
	//		return (glm::length(b - a) + glm::length(c - b) + glm::length(a - c)) / 3.0f;
	//		};
	//
	//	float ref_scale = avg_edge_len(DefaultP0, DefaultP1, DefaultP2);
	//	float def_scale = avg_edge_len(deformed[0], deformed[1], deformed[2]);
	//	float uniform_scale = def_scale / ref_scale;
	//
	//	// rot
	//	glm::mat3 rotation = def_basis * glm::transpose(ref_basis); // inverse of an orthonormal basis is its transpose
	//
	//	//
	//	m_Xform = glm::translate(m_Position) * glm::scale(glm::vec3{ uniform_scale }) * glm::mat4_cast(glm::quat_cast(rotation));
	//
	//	//
	//	//m_Xform = glm::translate(m_Position) * glm::mat4{ sclXrot };
	//}

	inline glm::vec3 GetNormal() const
	{
		return glm::normalize(glm::cross(m_P1 - m_P0, m_P2 - m_P0));
	}

	inline void UpdateCentroid()
	{
		static float one_third = 1.f / 3.f;
		glm::vec3 oldCentroid = m_Position;
		glm::vec3 p0_w = oldCentroid + m_P0;
		glm::vec3 p1_w = oldCentroid + m_P1;
		glm::vec3 p2_w = oldCentroid + m_P2;

		m_Position = one_third * (p0_w + p1_w + p2_w);

		// Update the local offsets since centroid changed
		m_P0 = p0_w - m_Position;
		m_P1 = p1_w - m_Position;
		m_P2 = p2_w - m_Position;
	}

	inline glm::vec3 const& GetP0() const { return m_P0; }
	inline glm::vec3& GetP0() { return m_P0; }
	inline glm::vec3 const& GetP1() const { return m_P1; }
	inline glm::vec3& GetP1() { return m_P1; }
	inline glm::vec3 const& GetP2() const { return m_P2; }
	inline glm::vec3& GetP2() { return m_P2; }

	inline glm::vec3 GetP0_W() { return m_Position + m_P0; }
	inline glm::vec3 GetP1_W() { return m_Position + m_P1; }
	inline glm::vec3 GetP2_W() { return m_Position + m_P2; }

private: // These points are OFFSETS from the centroid (position)
	glm::vec3 m_P0{ DefaultP0 };
	glm::vec3 m_P1{ DefaultP1 };
	glm::vec3 m_P2{ DefaultP2 };
};

class PlaneBV : public BaseBV
{
public:
	inline static const glm::vec3 DefaultNormal{ 0.f,0.f,1.f };
public:
	PlaneBV() = default;
	inline PlaneBV(glm::vec3 p, glm::vec3 const& eulerOrientation) : BaseBV(p), m_EulerOrientation(eulerOrientation) {}
	inline void UpdateXform() override
	{
		m_Xform = glm::translate(m_Position) * glm::scale(s_Scale) *
			glm::mat4_cast(glm::quat{ m_EulerOrientation });
	}

	inline glm::vec3 const& GetOrientation() const { return m_EulerOrientation; }
	inline glm::vec3& GetOrientation() { return m_EulerOrientation; }
	//inline glm::vec3 GetNormal() const
	//{
	//	glm::vec3 norm{
	//		glm::cos(m_EulerOrientation.y) * glm::cos(m_EulerOrientation.x),
	//		glm::sin(m_EulerOrientation.x),
	//		glm::sin(m_EulerOrientation.y) * glm::cos(m_EulerOrientation.x)
	//	};
	//	return glm::normalize(norm);
	//}
	// outward facing is the agreed upon standard for ray
	inline glm::vec3 GetNormal() const { return GetNormal(glm::quat{ m_EulerOrientation }); }
	inline float GetD() const { return glm::dot(GetNormal(), GetPosition()); }

public:
	inline static glm::vec3 GetNormal(glm::quat const& quat) { return quat * DefaultNormal; }

private:
	// Orientation of the normal
	glm::vec3 m_EulerOrientation{ 0.f,0.f,0.f }; // (Radians) Pitch, Yaw, Roll

	inline static glm::vec3 s_Scale{ 1.f, 1.f, 1.f };
};

class AABBBV : public BaseBV
{
public:
	AABBBV() = default;
	inline void UpdateXform() override
	{
		m_Xform = glm::translate(m_Position) * glm::scale(m_HalfExtents);
	}

	inline glm::vec3 const& GetHalfExtents() const { return m_HalfExtents; }
	inline glm::vec3& GetHalfExtents() { return m_HalfExtents; }
	inline glm::vec3 GetMinPoint() const { return m_Position - 0.5f * m_HalfExtents; }
	inline glm::vec3 GetMaxPoint() const { return m_Position + 0.5f * m_HalfExtents; }

private:
	glm::vec3 m_HalfExtents{ 1.f, 1.f, 1.f };

};

class SphereBV : public BaseBV
{
public:
	SphereBV() = default;
	inline SphereBV(glm::vec3 const& p, float r) : BaseBV(p), m_Radius(r) {}

	inline void UpdateXform() override
	{
		m_Xform = glm::translate(m_Position) * glm::scale(glm::vec3{ m_Radius });
	}

	inline float const& GetRadius() const { return m_Radius; }
	inline float& GetRadius() { return m_Radius; }

private:
	float m_Radius{ 1.f };
};
