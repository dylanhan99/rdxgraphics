#pragma once

class BaseBoundingVolume
{
public:
	~BaseBoundingVolume() = default;

	virtual void UpdateXform() = 0;

	inline glm::mat4 const& GetXform() const { return m_Xform; }
	inline glm::vec3 const& GetPosition() const { return m_Position; }
	inline glm::vec3& GetPosition() { return m_Position; }

protected:
	glm::mat4 m_Xform{};
	glm::vec3 m_Position{};
};

class Point : public BaseBoundingVolume
{
public:
	inline void UpdateXform() override
	{
		m_Xform = glm::translate(m_Position);
	}
};

class Ray : public BaseBoundingVolume
{
public:
	inline void UpdateXform() override
	{
		m_Xform = glm::translate(m_Position) * glm::scale(glm::vec3(s_Scale)) *
			glm::mat4_cast(glm::quat{ m_EulerOrientation });
	}

	inline glm::vec3& GetOrientation() { return m_EulerOrientation; }
private:
	glm::vec3 m_EulerOrientation{ 0.f,0.f,0.f }; // (Radians) Pitch, Yaw, Roll

	inline static float s_Scale{ 10.f };
};

class Plane : public BaseBoundingVolume
{
public:
	inline void UpdateXform() override
	{
		m_Xform = glm::translate(m_Position) * glm::scale(s_Scale) *
			glm::mat4_cast(glm::quat{ m_EulerOrientation });
	}

	inline glm::vec3& GetOrientation() { return m_EulerOrientation; }
	//inline glm::vec3& GetNormal() { return m_Normal; }

private:
	// Orientation of the normal
	glm::vec3 m_EulerOrientation{ 0.f,0.f,0.f }; // (Radians) Pitch, Yaw, Roll

	inline static glm::vec3 s_Scale{ 1.f, 1.f, 1.f };
};

class AABB : public BaseBoundingVolume
{
public:
	inline void UpdateXform() override
	{
		m_Xform = glm::translate(m_Position) * glm::scale(m_HalfExtents);
	}

private:
	glm::vec3 m_HalfExtents{ 1.f, 1.f, 1.f };
};

class Sphere : public BaseBoundingVolume
{
public:
	inline void UpdateXform() override
	{
		m_Xform = glm::translate(m_Position) * glm::scale(glm::vec3{ m_Radius });
	}

private:
	float m_Radius{ 1.f };
};
