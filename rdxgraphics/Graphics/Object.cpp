#include <pch.h>
#include "Object.h"

// Macro to explicitly init functions. Cus i'd rather have all defines in one place, than in both hpp and cpp.
#define _RX_XX(Klass, VKlass) \
	template class Object<Klass>;																 \
	template Object<Klass>& Object<Klass>::Push<VKlass>(typename VKlass::container_type const &); \
	template void Object<Klass>::BindInstancedData<VKlass>(GLsizeiptr offset, GLsizeiptr count); \
	template void Object<Klass>::EndObject();

#define _RX_X(Klass) _RX_XX(VertexBasic, Klass)
RX_VERTEX_BASIC_ATTRIBS;
#undef _RX_X
#define _RX_X(Klass) _RX_XX(VertexFBO, Klass)
RX_VERTEX_FBO_ATTRIBS;
#undef _RX_X

// Some initialization hack to setup vertex attrib IDs
VertexBasic _rx_hack_VertexBasic{};
VertexFBO _rx_hack_VertexFBO{};

template <typename T>
Object<T>::Object()
{
	m_VBOs.resize(typename T::Max());
	m_VBData.clear();
#define _RX_X(Klass) m_VBData.emplace_back(std::make_shared<Klass>());
	if constexpr (std::is_same_v<T, VertexBasic>)
	{
		RX_VERTEX_BASIC_ATTRIBS;
	}
	else if constexpr (std::is_same_v<T, VertexFBO>)
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
template <typename U>
std::enable_if_t<std::is_base_of_v<typename T::BaseAttribute, U>,
	void> Object<T>::BindInstancedData(GLsizeiptr offset, GLsizeiptr count)
{
	GLuint vbo = GetVBO<U>();
	typename U::container_type& pData = GetVBData<U>();

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(typename U::value_type), (void*)(pData.data() + offset), GL_DYNAMIC_DRAW);
}

template<typename T>
Object<T>& Object<T>::BeginObject(GLenum primitive)
{
	Terminate();
	m_Primitive = primitive;
	m_Index = 0;

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	return *this;
}

template<typename T>
void Object<T>::EndObject()
{
	// It's not instanced, we need PrimCount variable for draw call
	if (m_Indices.empty())
	{
		// Should be able to simply take the first object in m_VBData, ensure it's non-instanced attribute
#define _RX_X(Klass) { typename Klass::container_type& pData = GetVBData<Klass>(); m_PrimCount = pData.size(); break; }
		for (;;)
		{
			if constexpr (std::is_same_v<T, VertexBasic>)
			{
				RX_VERTEX_BASIC_ATTRIBS;
			}
			else if constexpr (std::is_same_v<T, VertexFBO>)
			{
				RX_VERTEX_FBO_ATTRIBS;
			}
			else
			{
				static_assert(false);
			}
		}
#undef _RX_X
	}
	else
	{
		m_PrimCount = m_Indices.size();
	}

	switch (m_Primitive)
	{
	case GL_POINTS: break;
	case GL_LINES: m_PrimCount /= 2; break;
	case GL_TRIANGLES: m_PrimCount /= 3; break;
	default:
		RX_ASSERT(false, "wut??");
		break;
	}
	glBindVertexArray(0);
}

template<typename T>
Object<T>& Object<T>::PushIndices(std::vector<GLuint> const& indices)
{
	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	m_Indices = indices;
	return *this;
}

template <typename T>
template <typename U>
std::enable_if_t<std::is_base_of_v<typename T::BaseAttribute, U>,
	Object<T>&> Object<T>::Push(typename U::container_type const& data)
{
	GLuint& vbo = m_VBOs[U::ID];

	// Init buffer
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	///////////////////////////////////////

	// Declare data buffer
	size_t len{};
	void* pData{};
	GLenum drawMode{};

	if (U::IsInstanced)
	{
		len = RX_MAX_INSTANCES;
		pData = nullptr;
		drawMode = GL_DYNAMIC_DRAW;
	}
	else
	{
		len = data.size();
		pData = (void*)data.data();
		drawMode = GL_STATIC_DRAW;
	}
	glBufferData(GL_ARRAY_BUFFER,
		len * sizeof(typename U::value_type),
		pData,
		drawMode);
	///////////////////////////////////////

	// Declare buffer layout
	uint32_t attribCount = GetAttribCount<typename U::value_type>();
	uint32_t fundamentalCount = GetFundamentalCount<typename U::attrib_type>();
	GLenum attribFundamentalT = GetFundamentalType<typename U::attrib_type>();
	GLenum norm = U::IsNormalized ? GL_TRUE : GL_FALSE;

	for (GLuint i = 0; i < attribCount; ++i)
	{
		size_t offset = sizeof(typename U::attrib_type) * i;

		glEnableVertexAttribArray(m_Index);
		glVertexAttribPointer(m_Index,
			fundamentalCount, attribFundamentalT, // For this attrib, how many of GLenums?
			norm,								  // Normalization
			sizeof(typename U::value_type),		  // Total attribute size
			(void*)offset);						  // Offset from start of the attribute

		if (U::IsInstanced)
			glVertexAttribDivisor(m_Index, 1);

		++m_Index;
	}
	///////////////////////////////////////

	GetVBData<U>() = data;
	return *this;
}

template <typename T>
void Object<T>::Draw(size_t count)
{
	if (IsIndexedMesh())
	{
		glDrawElementsInstanced(
			m_Primitive,
			m_Indices.size(),
			GL_UNSIGNED_INT,
			nullptr,
			count
		);
	}
	else
	{
		glDrawArraysInstanced(
			m_Primitive,
			0,
			GetPrimCount(),
			count
		);
	}
}

template<typename T>
void Object<T>::Flush()
{
#define _RX_X(Klass) if (Klass::IsInstanced) GetVBData<Klass>().clear();
	if constexpr (std::is_same_v<T, VertexBasic>)
	{
		RX_VERTEX_BASIC_ATTRIBS;
	}
	else if constexpr (std::is_same_v<T, VertexFBO>)
	{
		RX_VERTEX_FBO_ATTRIBS;
	}
	else
	{
		static_assert(false);
	}
#undef _RX_X
}
