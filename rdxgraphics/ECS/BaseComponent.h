#pragma once
#include <entt/entt.hpp>

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
	inline Xform(glm::vec3 const& pos) : m_Translate(pos) {}

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

class Collider : public BaseComponent
{
public:
	inline static const bool HasEnttHandle{ true };

public:
	Collider() = delete; // We want to force the entt::entity
	inline Collider(entt::entity handle) : m_Handle(handle) {};

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
	inline entt::entity GetEntityHandle() const { return m_Handle; }

private:
	BV m_BVType{ BV::NIL };
	entt::entity m_Handle{};
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
	inline static glm::vec3 GetDirection(glm::quat const& quat) { return quat * glm::vec3{ 0.f,0.f,-1.f }; }

private:
	glm::vec3 m_EulerOrientation{ 0.f,0.f,0.f }; // (Radians) Pitch, Yaw, Roll

	inline static float s_Scale{ 10.f };
};

class TriangleBV : public BaseBV
{
public:
	inline void UpdateXform() override
	{

	}

private:
};

class PlaneBV : public BaseBV
{
public:
	inline void UpdateXform() override
	{
		m_Xform = glm::translate(m_Position) * glm::scale(s_Scale) *
			glm::mat4_cast(glm::quat{ m_EulerOrientation });
	}

	inline glm::vec3 const& GetOrientation() const { return m_EulerOrientation; }
	inline glm::vec3& GetOrientation() { return m_EulerOrientation; }
	inline glm::vec3 GetNormal() const
	{
		glm::vec3 norm{
			glm::cos(m_EulerOrientation.y) * glm::cos(m_EulerOrientation.x),
			glm::sin(m_EulerOrientation.x),
			glm::sin(m_EulerOrientation.y) * glm::cos(m_EulerOrientation.x)
		};
		return glm::normalize(norm);
	}

private:
	// Orientation of the normal
	glm::vec3 m_EulerOrientation{ 0.f,0.f,0.f }; // (Radians) Pitch, Yaw, Roll

	inline static glm::vec3 s_Scale{ 1.f, 1.f, 1.f };
};

class AABBBV : public BaseBV
{
public:
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
