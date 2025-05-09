#include <pch.h>
#include "RenderPass.h"
#include "GLFWWindow/GLFWWindow.h"

bool RenderPass::Init(void*)
{
	m_FBO = 0;
	m_TextureBuffer = 0;
	m_WindowPos = { 0, 0 };
	m_BufferDims = GLFWWindow::GetWindowDims();

	return true;
}

bool RenderPass::Init(int x, int y, int width, int height)
{
	m_WindowPos = { x, y };
	m_BufferDims = { width, height };

	glCreateFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glGenTextures(1, &m_TextureBuffer);
	glBindTexture(GL_TEXTURE_2D, m_TextureBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureBuffer, 0);

	return true;
}

void RenderPass::Terminate()
{
	glDeleteFramebuffers(1, &m_FBO);
}

void RenderPass::BindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

void RenderPass::DrawThis(std::function<void()> drawStuff)
{
	BindFBO();
	glBindTexture(GL_TEXTURE_2D, m_TextureBuffer);
	//glViewport(m_WindowPos.x, m_WindowPos.y, m_BufferDims.x, m_BufferDims.y);
	drawStuff();
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
}