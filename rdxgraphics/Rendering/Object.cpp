#include <pch.h>
#include "Object.h"
Vertex _rx_hack_{};

bool Object::MakeObject(Object& oObject, GLenum primitive, std::vector<GLuint> const& indices,
	std::vector<Vertex::Position::value_type> const& positions)
{
	oObject.Terminate();
	oObject.m_Primitive = primitive;

	glGenVertexArrays(1, &oObject.m_VAO);
	glBindVertexArray(oObject.m_VAO);

	GLuint index = 0; // attribute index

	{ // Position buffer
		GLuint& vbo = oObject.m_VBOs[Vertex::Position::ID];
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, 
			positions.size() * sizeof(Vertex::Position::value_type),
			positions.data(), 
			GL_STATIC_DRAW);
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	++index;

	{ // Xform buffer (Instanced)
		GLuint& vbo = oObject.m_VBOs[Vertex::Xform::ID];
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER,
			RX_MAX_INSTANCES * sizeof(Vertex::Xform::value_type),
			nullptr,
			GL_DYNAMIC_DRAW);
		for (GLuint i = 0; i < Vertex::Xform::value_type::length(); ++i, ++index)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, 
				4, GL_FLOAT,					 // Referring to this attrib, being one of the vec4s
				GL_FALSE,						 // No normalization
				sizeof(glm::mat4),				 // Total attribute size
				(void*)(sizeof(glm::vec4) * i)); // Offset from start of the attribute
			glVertexAttribDivisor(index, 1);
		}
	}

	{ // Index buffer
		GLuint& ebo = oObject.m_EBO;
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	}

	//oObject.m_Positions = positions;
	oObject.m_Indices = indices;

	glBindVertexArray(0);
	return true;
}

Object::Object()
{
	m_VBOs.resize(Vertex::Max()); 
	m_VBData.clear();
#define _RX_X(Klass) m_VBData.emplace_back(std::make_shared<Klass>());
	RX_VERTEX_ATTRIBS
#undef _RX_X
}

void Object::Terminate()
{
	glDeleteVertexArrays(1, &m_VAO);
	m_VAO = 0;
	for (auto vbo : m_VBOs)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}
	glDeleteBuffers(1, &m_EBO);
	m_EBO = 0;
}

void Object::Bind()
{
	glBindVertexArray(m_VAO);
}

//void Object::BindInstancedData()
//{
//	// Bind instance xform data to the dynamic buffer
//	glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[Vertex::Xform::ID]);
//	glBufferData(GL_ARRAY_BUFFER, m_Xforms.size() * sizeof(Vertex::Xform::value_type), m_Xforms.data(), GL_DYNAMIC_DRAW);
//	//m_Xforms.clear();
//}

//void Object::Draw()
//{
//	glDrawElementsInstanced(
//		m_Primitive,
//		m_Indices.size(),
//		GL_UNSIGNED_INT,
//		nullptr,
//		m_Xforms.size() // Actual count of live instances
//	);
//}

void Object::Draw(size_t count)
{
	glDrawElementsInstanced(
		m_Primitive,
		m_Indices.size(),
		GL_UNSIGNED_INT,
		nullptr,
		count
	);
}
