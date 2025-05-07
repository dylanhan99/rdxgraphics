#pragma once

class BaseComponent
{
public:
	~BaseComponent() = default;
private:

};

class Model : public BaseComponent
{
public:
	Model() = default;
	void UpdateXform();

	inline void SetMesh(Rxuid uid) { m_MeshID = uid; }

private:
	glm::vec3 m_Translate{ 0.f }, m_Scale{ 1.f }, m_Rotate{ 0.f };
	glm::mat4 m_Xform{};

	Rxuid m_MeshID{ Rxuid{ Shape::NIL } };
	//Shape ShapeType{ Shape::Quad };
};