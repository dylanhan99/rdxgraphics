#pragma once
#include "GraphicsCommon.h"

class BaseUniformBuffer
{
public:
	~BaseUniformBuffer() = default;
	virtual size_t GetElementSize() const = 0;

	inline void Init(size_t count, GLuint defaultSlot)
	{
		InitSz(count * GetElementSize(), defaultSlot);
	}	

	inline void InitSz(size_t bufferSize, GLuint defaultSlot)
	{
		Terminate();
		m_BufferSize = (GLsizeiptr)bufferSize;
		m_Slot = defaultSlot;
		glGenBuffers(1, &m_UBO);
		Bind();
		glBufferData(GL_UNIFORM_BUFFER, m_BufferSize, nullptr, GL_STATIC_DRAW);
		Unbind();
	}

	inline void Submit(GLsizeiptr count, const void* pData) const
	{
		Submit(0, count, pData);
	}

	inline void Submit(GLintptr offsetCount, GLsizeiptr count, const void* pData) const
	{
		RX_ASSERT(pData, "Cannot be NULL");
		GLsizeiptr sz = count * GetElementSize();
		RX_ASSERT(sz <= (GLsizeiptr)GetBufferSize(), "Size is larger than buffer size.");
		GLsizeiptr offsetSz = offsetCount * GetElementSize();
		RX_ASSERT((offsetSz + sz) <= (GLsizeiptr)GetBufferSize(), "Size + offset is larger than buffer size.");

		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offsetSz, sz, pData);
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

	inline void BindBuffer() const { BindBuffer(m_Slot); }

	inline void Bind() const { glBindBuffer(GL_UNIFORM_BUFFER, m_UBO); }
	inline void Unbind() const { glBindBuffer(GL_UNIFORM_BUFFER, 0); }

	inline size_t GetBufferSize() const { return m_BufferSize; }

private:
	GLuint m_UBO{};
	GLsizeiptr m_BufferSize{};
	GLuint m_Slot{};
};

// This dosn't need to be a templated class at all, but i just wanted a way to validate within
// the engine, so we don't accidentally submit the wrong datatype
template <typename T>
class UniformBuffer : public BaseUniformBuffer
{
public:
	using value_type = T;

public:
	size_t GetElementSize() const override { return sizeof(value_type); }

	void Submit(GLsizeiptr count, const value_type* pData)
	{
		Submit(0, count, pData);
	}

	void Submit(GLintptr offsetCount, GLsizeiptr count, const value_type* pData)
	{
		BaseUniformBuffer::Submit(offsetCount, count, static_cast<const void*>(pData));
	}
};