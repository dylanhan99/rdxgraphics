#pragma once
#include "GraphicsCommon.h"

class BasePass
{
public:
	BasePass() = default;
	~BasePass() = default;
	inline BasePass(std::string const& name) : m_Name(name) {}

	bool Init(void*); // For final pass, no fbo, direct to screen.
	bool Init(int x, int y, int width, int height);
	void Terminate();

	void Draw() const;
	virtual void DrawImpl() const = 0;

	inline GLuint GetTextureBuffer() const { return m_TextureBuffer; }
	inline glm::ivec2 GetViewportDims() const { return m_ViewportDims; }
	inline glm::ivec2 GetBufferDims() const { return m_BufferDims; }
	inline glm::vec4& GetBackbufferColor() { return m_BackbufferColor; }

	inline std::string const& GetName() const { return m_Name; }

private:
	std::string m_Name{};

	GLuint m_FBO{};
	GLuint m_TextureBuffer{};
	GLuint m_DepthBuffer{};

	glm::ivec2 m_ViewportPos{}, m_ViewportDims{};
	glm::ivec2 m_BufferPos{}, m_BufferDims{};

	glm::vec4 m_BackbufferColor{ 0.2f, 0.3f, 0.3f, 0.f };
};