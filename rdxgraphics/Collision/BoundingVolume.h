#pragma once

class BaseBoundingVolume
{
public:
	BaseBoundingVolume() = default;
	~BaseBoundingVolume() = default;
	inline BaseBoundingVolume(glm::vec3 const& p) : m_Position(p) {}
	inline BaseBoundingVolume(float x, float y, float z) : m_Position({ x, y, z }) {}

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

class Point : public BaseBoundingVolume
{
public:
	Point() = default;
	inline Point(glm::vec3 const& p) : BaseBoundingVolume(p) {}
	inline Point(float x, float y, float z) : BaseBoundingVolume(x, y, z) {}

	inline void UpdateXform() override
	{
		m_Xform = glm::translate(m_Position);
	}
};

class Ray : public BaseBoundingVolume
{
public:
	Ray() = default;

	inline void UpdateXform() override
	{
		//RX_INFO("{} > {} > {}", GetDirection().x, GetDirection().y, GetDirection().z);
		m_Xform = glm::translate(m_Position) * glm::scale(glm::vec3{s_Scale}) *
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

class Triangle : public BaseBoundingVolume
{
public:
	Triangle() = default;

	inline void UpdateXform() override
	{

	}

private:
};

class Plane : public BaseBoundingVolume
{
public:
	Plane() = default;

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

class AABB : public BaseBoundingVolume
{
public:
	AABB() = default;

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

class Sphere : public BaseBoundingVolume
{
public:
	Sphere() = default;
	inline Sphere(glm::vec3 const& p, float r) : BaseBoundingVolume(p), m_Radius(r) {}

	inline void UpdateXform() override
	{
		m_Xform = glm::translate(m_Position) * glm::scale(glm::vec3{ m_Radius });
	}

	inline float const& GetRadius() const { return m_Radius; }
	inline float& GetRadius() { return m_Radius; }

private:
	float m_Radius{ 1.f };
};
