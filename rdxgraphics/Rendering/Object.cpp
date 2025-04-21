#include <pch.h>
#include "Object.h"

bool Object::MakeObject(Object& oObject, std::vector<GLuint> const& indices, 
	std::vector<Vertex::position_type> const& positions)
{
	glGenVertexArrays(1, &oObject.m_VAO);
	glBindVertexArray(oObject.m_VAO);

	GLuint index = 0; // attribute index

	{ // Position buffer
		GLuint& vbo = oObject.m_VBOs[(size_t)Vertex::Attribute::Position];
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, 
			positions.size() * sizeof(Vertex::position_type),
			positions.data(), 
			GL_STATIC_DRAW);
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	++index;

	{ // Xform buffer (Instanced)
		GLuint& vbo = oObject.m_VBOs[(size_t)Vertex::Attribute::Xform_inst];
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER,
			/*maxExpectedInstances*/1000 * sizeof(Vertex::xform_type),
			nullptr,
			GL_DYNAMIC_DRAW);
		auto d = Vertex::xform_type::length();
		for (GLuint i = 0; i < Vertex::xform_type::length(); ++i, ++index)
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

	oObject.m_Positions = positions;
	oObject.m_Indices = indices;

	glBindVertexArray(0);
	return true;
}

void Object::BindInstancedData()
{
	// Bind instance xform data to the dynamic buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[(size_t)Vertex::Attribute::Xform_inst]);
	glBufferData(GL_ARRAY_BUFFER, m_Xforms.size() * sizeof(Vertex::xform_type), m_Xforms.data(), GL_DYNAMIC_DRAW);
	//m_Xforms.clear();
}
