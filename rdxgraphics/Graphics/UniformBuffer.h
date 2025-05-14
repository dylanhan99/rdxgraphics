#pragma once
#include "GraphicsCommon.h"

class BaseUniformBuffer
{
public:
	~BaseUniformBuffer() = default;
	virtual void Init(size_t count) = 0;
	inline void InitSz(GLsizeiptr bufferSize)
	{
		Terminate();
		m_BufferSize = bufferSize;
		glGenBuffers(1, &m_UBO);
		Bind();
		glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
		Unbind();
	}

	inline void Terminate()
	{
		glDeleteBuffers(1, &m_UBO);
		m_UBO = 0;
	}

	// Slot must be less than max avail slots
	inline void BindBuffer(GLuint slot) const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, slot, m_UBO);
	}

	inline void Bind() const { glBindBuffer(GL_UNIFORM_BUFFER, m_UBO); }
	inline void Unbind() const { glBindBuffer(GL_UNIFORM_BUFFER, 0); }

	inline size_t GetBufferSize() const { return m_BufferSize; }

private:
	GLuint m_UBO{};
	GLsizeiptr m_BufferSize{};
};

// This dosn't need to be a templated class at all, but i just wanted a way to validate within
// the engine, so we don't accidentally submit the wrong datatype
template <typename T>
class UniformBuffer : public BaseUniformBuffer
{
public:
	using value_type = T;
public:
	void Init(size_t count)
	{
		InitSz(count * sizeof(value_type));
	}

	void Submit(GLsizeiptr count, const value_type* pData)
	{
		Submit(0, count, pData);
	}

	void Submit(GLintptr offsetCount, GLsizeiptr count, const value_type* pData)
	{
		RX_ASSERT(pData, "Cannot be NULL");
		GLsizeiptr sz = count * sizeof(value_type);
		RX_ASSERT(sz <= GetBufferSize(), "Size is larger than buffer size.");
		GLsizeiptr offsetSz = offsetCount * sizeof(value_type);
		RX_ASSERT((offsetSz + sz) <= GetBufferSize(), "Size + offset is larger than buffer size.");

		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offsetSz, sz, pData);
		Unbind();
	}

private:
};