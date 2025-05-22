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
	F_O_O(Metadata, ##__VA_ARGS__);		   \
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
