#include <pch.h>
#include "Object.h"
template class Object<VertexBasic>;
template class Object<VertexFBO>;

// Some initialization hack to setup vertex attrib IDs
VertexBasic _rx_hack_vxb{};
VertexFBO _rx_hack_vxf{};

template <typename T>
template <typename U>
std::enable_if_t<std::is_base_of_v<typename T::BaseAttribute, U>,
	Object<T>&> Object<T>::Push<U>(typename T::container_type const& data)
{
	GLuint m_Index = 0; // attribute index // find m_Index in stash. need to reset in begin()
	GLuint& vbo = m_VBOs[U::ID];

	///////////////////////////////////////
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	///////////////////////////////////////

	///////////////////////////////////////
	size_t len{};
	void* pData{};
	GLenum drawMode{};

	if (U::IsInstanced)
	{
		len = RX_MAX_INSTANCES;
		pData = nullptr;
		draw = GL_DYNAMIC_DRAW;
	}
	else
	{
		len = data.size();
		pData = data.data()
		draw = GL_STATIC_DRAW;
	}
	glBufferData(GL_ARRAY_BUFFER,
		len * sizeof(typename U::value_type),
		pData,
		drawMode);
	///////////////////////////////////////

	///////////////////////////////////////

	uint32_t attribCount = GetAttribCount<typename U::value_type>(); // How many attrib_type make up value_type?			4*vec4 in a mat4
	uint32_t fundamentalCount = GetFundamentalCount<typename U::attrib_type>(); // How many base types make up attrib_type? 4*float in a vec4
	GLenum attribFundamentalT = GetFundamentalType<typename U::attrib_type>();
	GLenum norm = U::IsNormalized ? GL_TRUE : GL_FALSE;

	for (GLuint i = 0; i < U::value_type::length(); ++i, ++m_Index)
	{
		size_t offset = (void*)(sizeof(typename U::attrib_type) * i);

		glEnableVertexAttribArray(m_Index);
		glVertexAttribPointer(m_Index,
			attribCount, attribFundamentalT,	// Referring to this attrib, being one of the vec4s
			norm,								// No normalization
			sizeof(typename U::value_type),		// Total attribute size
			offset);	// Offset from start of the attribute
		glVertexAttribDivisor(m_Index, 1);
	}
	///////////////////////////////////////


	return *this;
}

template <typename T>
bool Object<T>::MakeObject(Object& oObject, GLenum primitive, std::vector<GLuint> const& indices,
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

template <typename T>
Object<T>::Object()
{
	m_VBOs.resize(typename vertex_type::Max());
	m_VBData.clear();
#define _RX_X(Klass) m_VBData.emplace_back(std::make_shared<Klass>());
	if constexpr (std::is_same_v<typename vertex_type, VertexBasic))
	{
		RX_VERTEX_BASIC_ATTRIBS;
	}
	else if constexpr (std::is_same_v<typename vertex_type, VertexFBO))
	{
		RX_VERTEX_FBO_ATTRIBS;
	}
	else
	{
		static_assert(false);
	}
#undef _RX_X
}

template <typename T>
void Object<T>::Terminate()
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

template <typename T>
void Object<T>::Bind()
{
	glBindVertexArray(m_VAO);
}

template <typename T>
void Object<T>::Draw(size_t count)
{
	glDrawElementsInstanced(
		m_Primitive,
		m_Indices.size(),
		GL_UNSIGNED_INT,
		nullptr,
		count
	);
}
