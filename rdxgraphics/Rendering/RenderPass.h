#pragma once

class RenderPass
{
public:
	bool Init(void*); // For final pass, no fbo, direct to screen.
	bool Init(int width, int height);
	void Terminate();

	// Temporary, while Im still figuring out how scenes/passes will interact
	inline void BindFBO() { glBindFramebuffer(GL_FRAMEBUFFER, m_FBO); };

	// maybe something like this will be how drawing could take place (flow-wise)
	void DrawThis(std::function<void()> drawStuff)
	{
		BindFBO();
		glBindTexture(GL_TEXTURE_2D, m_TextureBuffer);
		drawStuff();
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	}

public:
	GLuint m_FBO{};
	GLuint m_TextureBuffer{};
};