#pragma once
#include <entt/entt.hpp>

// These cover all "main" components. 
// In the context of collider, Collider is the "main", while _BV variants are the "subsidiaries"
#define RX_DO_MAIN_COMPONENTS_M(F_O_O, ...)\
	F_O_O(Metadata, ##__VA_ARGS__);		   \
	F_O_O(Xform, ##__VA_ARGS__);		   \
	F_O_O(Camera, ##__VA_ARGS__);		   \
	F_O_O(Model, ##__VA_ARGS__);		   \
	F_O_O(DirectionalLight, ##__VA_ARGS__);\
	F_O_O(Material, ##__VA_ARGS__);		   \
	F_O_O(Collider, ##__VA_ARGS__);		   \
	F_O_O(BoundingVolume, ##__VA_ARGS__);

#define RX_DO_MAIN_COMPONENTS RX_DO_MAIN_COMPONENTS_M(_RX_X)

class BaseComponent
{
public:
	~BaseComponent() = default;
	inline virtual void OnConstructImpl() {};
private:
};

#define RX_COMPONENT_DEC_HANDLE												\
public:																		\
	inline entt::entity GetEntityHandle() const { return m_Handle; }		\
	inline void SetEntityHandle(entt::entity handle) { m_Handle = handle; } \
private:																	\
	entt::entity m_Handle{};

#define RX_COMPONENT_DEF_HANDLE(Klass)											 \
private:																		 \
	inline static void OnConstruct(entt::registry& registry, entt::entity handle)\
	{																			 \
		Klass& klass = registry.get<Klass>(handle);								 \
		klass.SetEntityHandle(handle);											 \
		klass.OnConstructImpl();												 \
	}																			 \
public:																			 \
	inline static void Init(entt::registry& registry)							 \
	{																			 \
		registry.on_construct<Klass>().connect<&Klass::OnConstruct>();			 \
		registry.on_update<Klass>().connect<&Klass::OnConstruct>();				 \
	}																			 \
private:

#define RX_COMPONENT_HAS_HANDLE(Klass) \
RX_COMPONENT_DEC_HANDLE;			   \
RX_COMPONENT_DEF_HANDLE(Klass);

class Xform : public BaseComponent
{
	RX_COMPONENT_HAS_HANDLE(Xform);
public:
	class Dirty : public BaseComponent { char _{}; };

public:
	Xform() = default;
	inline Xform(glm::vec3 const& pos, glm::vec3 scale = glm::vec3{1.f}, glm::vec3 eulOri = glm::vec3{0.f})
		: m_Translate(pos), m_Scale(scale), m_Rotate(eulOri) { }

	void UpdateXform();

	inline glm::mat4 const& GetXform() const { return m_Xform; }
	inline glm::mat4& GetXform() { return m_Xform; }
	inline glm::vec3 const& GetTranslate() const { return m_Translate; }
	inline glm::vec3 const& GetScale() const { return m_Scale; }
	inline glm::vec3 const& GetEulerOrientation() const { return m_Rotate; }
	inline glm::mat4 GetRotationMatrix() const { return static_cast<glm::mat4>(glm::quat(m_Rotate)); }
	glm::vec3& GetTranslate();
	glm::vec3& GetScale();
	glm::vec3& GetEulerOrientation();
	void SetTranslate(glm::vec3);
	void SetScale(glm::vec3);
	void SetEulerOrientation(glm::vec3);

private:
	inline void OnConstructImpl() { SetDirty(); }
	void SetDirty() const; // Indicates this xform to be dirty and m_Xform MUST be updated.

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
	inline Material(glm::vec3 diffuseColor, float diffuseIntensity = 1.f) 
		: m_DiffuseColor(diffuseColor), m_DiffuseIntensity(diffuseIntensity) {}

	inline glm::vec3 const& GetDiffuseColor() const { return m_DiffuseColor; }
	inline glm::vec3& GetDiffuseColor() { return m_DiffuseColor; }
	inline float const& GetDiffuseIntensity() const { return m_DiffuseIntensity; }
	inline float& GetDiffuseIntensity() { return m_DiffuseIntensity; }

	inline operator glm::vec4() const
	{
		return glm::vec4{ m_DiffuseColor.x, m_DiffuseColor.y, m_DiffuseColor.z, m_DiffuseIntensity };
	}

private:
	glm::vec3 m_DiffuseColor{ 0.f };
	float m_DiffuseIntensity{ 0.f };
};
