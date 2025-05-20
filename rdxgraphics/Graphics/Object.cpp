#include <pch.h>
#include "Object.h"

// Macro to explicitly init functions. Cus i'd rather have all defines in one place, than in both hpp and cpp.
#define _RX_XX(VKlass, Klass) \
	template Object<Klass>& Object<Klass>::Push<VKlass>(typename VKlass::container_type const &); \
	template void Object<Klass>::BindInstancedData<VKlass>(GLsizeiptr offset, GLsizeiptr count);
#define _RX_XXX(Klass) m_VBData.emplace_back(std::make_shared<Klass>());
#define _RX_XXXX(Klass) { typename Klass::container_type& pData = GetVBData<Klass>(); m_PrimCount = pData.size(); break; }
#define _RX_XXXXX(Klass) if (Klass::IsInstanced) GetVBData<Klass>().clear();

#define _RX_X(Klass, _RX_FOR_EACH)				\
	template class Object<Klass>;				\
	Klass _rx_hack_##Klass{};					\
	template void Object<Klass>::EndObject();	\
	template<> Object<Klass>::Object()			\
	{											\
		m_VBOs.resize(Klass::Max());			\
		m_VBData.clear();						\
		_RX_FOR_EACH(_RX_XXX);					\
	}											\
	_RX_FOR_EACH(_RX_XX, Klass)																						\
	template<> void Object<Klass>::EndObject()																		\
	{																												\
		/* It's not instanced, we need PrimCount variable for draw call */											\
		if (m_Indices.empty())																						\
		{																											\
			/* Should be able to simply take the first object in m_VBData, ensure it's non-instanced attribute */	\
			for (;;) { _RX_FOR_EACH(_RX_XXXX); }																	\
		}																											\
		else																										\
		{																											\
			m_PrimCount = (GLsizei)m_Indices.size();																\
		}																											\
		glBindVertexArray(0);																						\
	}																												\
	template<> void Object<Klass>::Flush() { _RX_FOR_EACH(_RX_XXXXX); }

_RX_X(VertexBasic, RX_VERTEX_BASIC_ATTRIBS_M);
_RX_X(VertexFBO, RX_VERTEX_FBO_ATTRIBS_M);
#undef _RX_XXXXX
#undef _RX_XXXX
#undef _RX_XXX
#undef _RX_XX
#undef _RX_X

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
			(GLsizei)count
		);
	}
	else
	{
		glDrawArraysInstanced(
			m_Primitive,
			0,
			GetPrimCount(),
			(GLsizei)count
		);
	}
}
