#pragma once

// Representation of a model
class Object
{
public:
	static bool MakeObject(Object& oObject, std::vector<GLuint> const& indices,
		std::vector<Vertex::position_type> const& positions);
public:
	inline void Submit(glm::mat4 xform = glm::translate(glm::vec3(0.f))) { m_Xforms.emplace_back(std::move(xform)); }
	void BindInstancedData();

public:
	GLuint m_VAO{};
	std::array<GLuint, (size_t)Vertex::Attribute::MAX> m_VBOs{};
	GLuint m_EBO{};

	std::vector<GLuint> m_Indices{};
	std::vector<Vertex::position_type> m_Positions{};
	std::vector<glm::mat4> m_Xforms{};
};