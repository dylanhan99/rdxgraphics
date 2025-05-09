#pragma once
#include "GraphicsCommon.h"

class RenderPass
{
public:
	bool Init(void*); // For final pass, no fbo, direct to screen.
	bool Init(int x, int y, int width, int height);
	void Terminate();

	// Temporary, while Im still figuring out how scenes/passes will interact
	void BindFBO();

	// maybe something like this will be how drawing could take place (flow-wise)
	void DrawThis(std::function<void()> drawStuff);

public:
	GLuint m_FBO{};
	GLuint m_TextureBuffer{};

	glm::ivec2 m_WindowPos{};
	glm::ivec2 m_BufferDims{};
};