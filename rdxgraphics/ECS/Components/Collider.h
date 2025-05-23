#pragma once
#include "BaseComponent.h"

class Collider : public BaseComponent
{
	RX_COMPONENT_HAS_HANDLE(Collider);
public:
	inline Collider(entt::entity handle, Primitive primType) : Collider(handle) { SetPrimitiveType(primType); }

	inline Primitive GetPrimitiveType() const { return m_PrimitiveType; }
	void SetPrimitiveType(Primitive primType);
	glm::vec3 RemovePrimitive(); // returns position of removed BV

private:
	Primitive m_PrimitiveType{ Primitive::NIL };
};

class BasePrimitive : public BaseComponent
{
public:
	BasePrimitive() = default;
	~BasePrimitive() = default;
	inline BasePrimitive(glm::vec3 const& p) : m_Position(p) {}
	inline BasePrimitive(float x, float y, float z) : m_Position({ x, y, z }) {}

	virtual void UpdateXform() = 0;

	inline glm::mat4 const& GetXform() const { return m_Xform; }
	inline glm::vec3 const& GetPosition() const { return m_Position; }
	inline glm::vec3& GetPosition() { return m_Position; }
	inline void SetPosition(glm::vec3 pos) { m_Position = pos; }
	inline bool const& IsCollide() const { return m_IsCollide; }
	inline bool& IsCollide() { return m_IsCollide; }
	inline bool const& IsFollowXform() const { return m_IsFollowXform; }
	inline bool& IsFollowXform() { return m_IsFollowXform; }

protected:
	glm::mat4 m_Xform{};
	glm::vec3 m_Position{};
	bool m_IsCollide{ false };
	bool m_IsFollowXform{ true };
};

class PointPrimitive : public BasePrimitive
{
public:
	PointPrimitive() = default;
	inline PointPrimitive(glm::vec3 const& p) : BasePrimitive(p) {}
	inline PointPrimitive(float x, float y, float z) : BasePrimitive(x, y, z) {}

	inline void UpdateXform() override
	{
		m_Xform = glm::translate(m_Position);
	}
};

class RayPrimitive : public BasePrimitive
{
public:
	inline static const glm::vec3 DefaultDirection{ 0.f,0.f,-1.f };
public:
	RayPrimitive() = default;
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

	inline static float s_Scale{ 100.f };
};

// Must be CCW orientation
class TrianglePrimitive : public BasePrimitive
{
public:
	inline static const glm::vec3 DefaultP0{ 0.0f,		 1.0f, 0.f };
	inline static const glm::vec3 DefaultP1{ -0.86603f,	-0.5f, 0.f };
	inline static const glm::vec3 DefaultP2{ 0.86603f,	-0.5f, 0.f };
	inline static const glm::vec3 DefaultNormal{ glm::normalize(glm::cross(DefaultP1 - DefaultP0, DefaultP2 - DefaultP0)) };
public:
	TrianglePrimitive() = default;
	inline void UpdateXform() override
	{
		m_Xform = glm::translate(GetPosition());
	}

	inline glm::vec3 GetNormal_NotNormalized() const
	{
		return glm::cross(m_P1 - m_P0, m_P2 - m_P0);
	}

	inline glm::vec3 GetNormal() const
	{
		return glm::normalize(GetNormal_NotNormalized());
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

	inline glm::vec3 GetP0_W() const { return m_Position + m_P0; }
	inline glm::vec3 GetP1_W() const { return m_Position + m_P1; }
	inline glm::vec3 GetP2_W() const { return m_Position + m_P2; }

private: // These points are OFFSETS from the centroid (position)
	glm::vec3 m_P0{ DefaultP0 };
	glm::vec3 m_P1{ DefaultP1 };
	glm::vec3 m_P2{ DefaultP2 };
};

class PlanePrimitive : public BasePrimitive
{
public:
	inline static const glm::vec3 DefaultNormal{ 0.f,0.f,1.f };
	inline static const uint32_t DefaultSize{ 40 };
public:
	PlanePrimitive() = default;
	inline PlanePrimitive(glm::vec3 p, glm::vec3 const& eulerOrientation) : BasePrimitive(p), m_EulerOrientation(eulerOrientation) {}
	inline void UpdateXform() override
	{
		m_Xform = glm::translate(m_Position) * glm::scale(s_Scale) *
			glm::mat4_cast(glm::quat{ m_EulerOrientation });
	}

	inline glm::vec3 const& GetOrientation() const { return m_EulerOrientation; }
	inline glm::vec3& GetOrientation() { return m_EulerOrientation; }

	// outward facing is the agreed upon standard for ray
	inline glm::vec3 GetNormal() const { return GetNormal(glm::quat{ m_EulerOrientation }); }
	inline float GetD() const { return glm::dot(GetNormal(), GetPosition()); }

public:
	inline static glm::vec3 GetNormal(glm::quat const& quat) { return quat * DefaultNormal; }

private:
	// Orientation of the normal
	glm::vec3 m_EulerOrientation{ 0.f,0.f,0.f }; // (Radians) Pitch, Yaw, Roll

	inline static glm::vec3 s_Scale{ 2.f };
};

class AABBPrimitive : public BasePrimitive
{
public:
	AABBPrimitive() = default;
	inline void UpdateXform() override
	{
		m_Xform = glm::translate(m_Position) * glm::scale(m_HalfExtents);
	}

	inline glm::vec3 const& GetHalfExtents() const { return m_HalfExtents; }
	inline glm::vec3& GetHalfExtents() { return m_HalfExtents; }
	inline glm::vec3 GetMinPoint() const { return m_Position - 0.5f * m_HalfExtents; }
	inline glm::vec3 GetMaxPoint() const { return m_Position + 0.5f * m_HalfExtents; }

private:
	glm::vec3 m_HalfExtents{ 0.5f };

};

class SpherePrimitive : public BasePrimitive
{
public:
	SpherePrimitive() = default;
	inline SpherePrimitive(glm::vec3 const& p, float r) : BasePrimitive(p), m_Radius(r) {}

	inline void UpdateXform() override
	{
		m_Xform = glm::translate(m_Position) * glm::scale(glm::vec3{ m_Radius });
	}

	inline float const& GetRadius() const { return m_Radius; }
	inline float& GetRadius() { return m_Radius; }

private:
	float m_Radius{ 1.f };
};
