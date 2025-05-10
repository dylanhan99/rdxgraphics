#include <pch.h>
#include "RenderPass.h"
#include "GLFWWindow/GLFWWindow.h"

bool RenderPass::Init(void*)
{
	m_FBO = 0;
	m_TextureBuffer = 0;
	m_ViewportPos = m_BufferPos = { 0, 0 };
	m_ViewportDims = m_BufferDims = GLFWWindow::GetWindowDims();

	return true;
}

bool RenderPass::Init(int x, int y, int width, int height)
{
	m_ViewportPos = { x, y };
	m_ViewportDims = { width, height };

	m_BufferPos = { 0, 0 };
	m_BufferDims = GLFWWindow::GetWindowDims();

	glCreateFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glGenTextures(1, &m_TextureBuffer);
	glBindTexture(GL_TEXTURE_2D, m_TextureBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_BufferDims.x, m_BufferDims.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureBuffer, 0);

	// Create a depth buffer (needed for depth testing)
	//GLuint depthBuffer;
	//glGenRenderbuffers(1, &depthBuffer);
	//glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_BufferDims.x, m_BufferDims.y); // 24-bit depth buffer
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	return true;
}

void RenderPass::Terminate()
{
	glDeleteFramebuffers(1, &m_FBO);
	m_FBO = 0;
}

void RenderPass::DrawThis(std::function<void()> drawStuff)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);	
	glBindTexture(GL_TEXTURE_2D, m_TextureBuffer);
	glViewport(m_ViewportPos.x, m_ViewportPos.y, m_ViewportDims.x, m_ViewportDims.y);
	drawStuff();
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
}