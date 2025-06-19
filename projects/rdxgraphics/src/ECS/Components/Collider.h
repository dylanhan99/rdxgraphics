#pragma once
#include "BaseComponent.h"

class Collider : public BaseComponent
{
	RX_COMPONENT_HAS_HANDLE(Collider);
public:
	class DirtyXform : public BaseComponent { char _{}; };

public:
	inline Collider(Primitive primType = Primitive::NIL) : m_PrimitiveType(primType) { }
	inline void OnConstructImpl() override { SetupPrimitive(); }
	inline void OnDestroyImpl() override { SetPrimitiveType(Primitive::NIL); }

	inline Primitive GetPrimitiveType() const { return m_PrimitiveType; }
	void SetPrimitiveType(Primitive primType);

private:
	void SetupPrimitive(glm::vec3 offset = glm::vec3{0.f}) const;
	glm::vec3 RemovePrimitive(); // returns offset of previous primitive

private:
	Primitive m_PrimitiveType{ Primitive::NIL };
};

class BasePrimitive : public BaseComponent
{
	RX_COMPONENT_DEC_HANDLE;
public:
	BasePrimitive() = default;
	~BasePrimitive() = default;
	inline BasePrimitive(glm::vec3 const& o) : m_Offset(o) {}
	inline BasePrimitive(float x, float y, float z) : m_Offset({ x, y, z }) {}

	// Can be overrided again if needed, but call BasePrimitive::OnConstructImpl() first
	inline void OnConstructImpl() override { SetDirtyXform(); }
	virtual void UpdateXform() = 0;
	virtual void SetDirtyXform() const;

	inline glm::mat4 const& GetXform() const { return m_Xform; }
	inline glm::vec3 const& GetOffset() const { return m_Offset; }
	inline glm::vec3& GetOffset() { SetDirtyXform(); return m_Offset; }
	inline void SetOffset(glm::vec3 o) { SetDirtyXform(); m_Offset = o; }

	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 pos);

	inline bool const& IsCollide() const { return m_IsCollide; }
	inline bool& IsCollide() { return m_IsCollide; }

protected:
	glm::mat4 m_Xform{};

private:
	glm::vec3 m_Offset{};
	bool m_IsCollide{ false };
};

class PointPrimitive : public virtual BasePrimitive
{
	RX_COMPONENT_DEF_HANDLE(PointPrimitive);
public:
	PointPrimitive() = default;
	inline PointPrimitive(glm::vec3 const& o) : BasePrimitive(o) {}
	inline PointPrimitive(float x, float y, float z) : BasePrimitive(x, y, z) {}

	inline void UpdateXform() override
	{
		m_Xform = glm::translate(GetPosition());
	}
};

class RayPrimitive : public virtual BasePrimitive
{
	RX_COMPONENT_DEF_HANDLE(RayPrimitive);
public:
	inline static const glm::vec3 DefaultDirection{ 0.f,0.f,-1.f };
public:
	RayPrimitive() = default;
	inline void UpdateXform() override
	{
		m_Xform = 
			glm::translate(GetPosition()) * 
			glm::mat4_cast(GetOrientationQuat()) * 
			glm::scale(glm::vec3{ s_Scale });
	}

	inline glm::vec3 const& GetOrientation() const { return m_EulerOrientation; }
	inline glm::vec3& GetOrientation() { return m_EulerOrientation; }
	inline glm::quat GetOrientationQuat() const { return std::move(glm::quat{ m_EulerOrientation }); }

	// inward facing is the agreed upon standard for ray
	inline glm::vec3 GetDirection() const { return GetDirection(glm::quat{ m_EulerOrientation }); }

	// direction MUST be normalized
	inline void SetDirection(glm::vec3 const& direction) { m_EulerOrientation = glm::eulerAngles(glm::rotation(DefaultDirection, direction)); }

public:
	inline static glm::vec3 GetDirection(glm::quat const& quat) { return quat * DefaultDirection; }

private:
	glm::vec3 m_EulerOrientation{ 0.f,0.f,0.f }; // (Radians) Pitch, Yaw, Roll

	inline static float s_Scale{ 100.f };
};

// Must be CCW orientation
class TrianglePrimitive : public virtual BasePrimitive
{
	RX_COMPONENT_DEF_HANDLE(TrianglePrimitive);
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
		glm::vec3 oldCentroid = GetPosition();
		glm::vec3 p0_w = oldCentroid + m_P0;
		glm::vec3 p1_w = oldCentroid + m_P1;
		glm::vec3 p2_w = oldCentroid + m_P2;
		
		glm::vec3 newCentroid = one_third * (p0_w + p1_w + p2_w);
		SetPosition(newCentroid); // Updates m_Offset
		
		// Update the local offsets since centroid changed
		m_P0 = p0_w - newCentroid;
		m_P1 = p1_w - newCentroid;
		m_P2 = p2_w - newCentroid;
	}

	inline glm::vec3 const& GetP0() const { return m_P0; }
	inline glm::vec3& GetP0() { return m_P0; }
	inline glm::vec3 const& GetP1() const { return m_P1; }
	inline glm::vec3& GetP1() { return m_P1; }
	inline glm::vec3 const& GetP2() const { return m_P2; }
	inline glm::vec3& GetP2() { return m_P2; }

	inline glm::vec3 GetP0_W() const { return GetPosition() + m_P0; }
	inline glm::vec3 GetP1_W() const { return GetPosition() + m_P1; }
	inline glm::vec3 GetP2_W() const { return GetPosition() + m_P2; }

private: // These points are OFFSETS from the centroid (position)
	glm::vec3 m_P0{ DefaultP0 };
	glm::vec3 m_P1{ DefaultP1 };
	glm::vec3 m_P2{ DefaultP2 };
};

class PlanePrimitive : public virtual BasePrimitive
{
	RX_COMPONENT_DEF_HANDLE(PlanePrimitive);
public:
	inline static const glm::vec3 DefaultNormal{ 0.f,0.f,1.f };
	inline static const uint32_t DefaultSize{ 40 };
public:
	PlanePrimitive() = default;
	inline PlanePrimitive(glm::vec3 o, glm::vec3 const& eulerOrientation) : BasePrimitive(o), m_EulerOrientation(eulerOrientation) {}
	inline void UpdateXform() override
	{
		m_Xform = glm::translate(GetPosition()) *
			glm::mat4_cast(glm::quat{ m_EulerOrientation }) * 
			glm::scale(s_Scale);
	}

	inline glm::vec3 const& GetOrientation() const { return m_EulerOrientation; }
	inline glm::vec3& GetOrientation() { return m_EulerOrientation; }

	// outward facing is the agreed upon standard for ray
	inline glm::vec3 GetNormal() const { return GetNormal(glm::quat{ m_EulerOrientation }); }
	inline float GetD() const { return glm::dot(GetNormal(), GetPosition()); }

	inline glm::vec4 GetPlaneEquation() const { return glm::vec4{ GetNormal(), GetD() }; }

public:
	inline static glm::vec3 GetNormal(glm::quat const& quat) { return quat * DefaultNormal; }

private:
	// Orientation of the normal
	glm::vec3 m_EulerOrientation{ 0.f,0.f,0.f }; // (Radians) Pitch, Yaw, Roll

	inline static glm::vec3 s_Scale{ 2.f };
};

class AABBPrimitive : public virtual BasePrimitive
{
	RX_COMPONENT_DEF_HANDLE(AABBPrimitive);
public:
	AABBPrimitive() = default;
	~AABBPrimitive() = default;

	inline void UpdateXform() override
	{
		m_Xform = glm::translate(GetPosition()) * glm::scale(2.f * m_HalfExtents);
	}

	inline glm::vec3 const& GetHalfExtents() const { return m_HalfExtents; }
	inline glm::vec3& GetHalfExtents() { return m_HalfExtents; }
	inline virtual glm::vec3 GetMinPoint() const { return GetPosition() - m_HalfExtents; }
	inline virtual glm::vec3 GetMaxPoint() const { return GetPosition() + m_HalfExtents; }

private:
	glm::vec3 m_HalfExtents{ 0.5f };
};

class SpherePrimitive : public virtual BasePrimitive
{
	RX_COMPONENT_DEF_HANDLE(SpherePrimitive);
public:
	SpherePrimitive() = default;
	~SpherePrimitive() = default;

	inline SpherePrimitive(glm::vec3 const& o, float r) : BasePrimitive(o), m_Radius(r) {}

	inline void UpdateXform() override
	{
		m_Xform = glm::translate(GetPosition()) * glm::scale(glm::vec3{ m_Radius });
	}

	inline float const& GetRadius() const { return m_Radius; }
	inline float& GetRadius() { return m_Radius; }

private:
	float m_Radius{ 1.f };
};
