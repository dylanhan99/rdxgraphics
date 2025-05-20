#pragma once
#include "GraphicsCommon.h"

class RenderPass
{
public:
	bool Init(void*); // For final pass, no fbo, direct to screen.
	bool Init(int x, int y, int width, int height);
	void Terminate();

	// maybe something like this will be how drawing could take place (flow-wise)
	void DrawThis(std::function<void()> drawStuff) const;

	inline GLuint GetTextureBuffer() const { return m_TextureBuffer; }
	inline glm::ivec2 GetViewportDims() const { return m_ViewportDims; }
	inline glm::ivec2 GetBufferDims() const { return m_BufferDims; }

	inline glm::vec4& GetBackbufferColor() { return m_BackbufferColor; }

private:
	GLuint m_FBO{};
	GLuint m_TextureBuffer{};
	GLuint m_DepthBuffer{};

	glm::ivec2 m_ViewportPos{}, m_ViewportDims{};
	glm::ivec2 m_BufferPos{}, m_BufferDims{};

	glm::vec4 m_BackbufferColor{ 0.2f, 0.3f, 0.3f, 0.f };
};