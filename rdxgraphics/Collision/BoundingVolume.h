#pragma once

class BaseBoundingVolume
{
public:
	~BaseBoundingVolume() = default;

	virtual void UpdateXform() = 0;
	inline glm::mat4 const& GetXform() const { return m_Xform; }

protected:
	glm::mat4 m_Xform{};
	glm::vec3 m_Position{};
};

class Point : public BaseBoundingVolume
{
public:
	inline void UpdateXform() override
	{
		m_Xform = glm::mat4(glm::translate(m_Position));
	}
};
//
//class Ray : public BaseBoundingVolume
//{
//};
//
//class Plane : public BaseBoundingVolume
//{
//};
//
class AABB : public BaseBoundingVolume
{
public:
	inline void UpdateXform() override
	{
		m_Xform = glm::mat4(glm::translate(m_Position)) * glm::mat4(glm::scale(m_HalfExtents));
	}

private:
	glm::vec3 m_HalfExtents{ 1.f, 1.f, 1.f };
};

class Sphere : public BaseBoundingVolume
{
public:
	inline void UpdateXform() override
	{
		m_Xform = glm::mat4(glm::translate(m_Position)) * glm::mat4(glm::scale(glm::vec3{ m_Radius }));
	}

private:
	float m_Radius{ 1.f };
};
