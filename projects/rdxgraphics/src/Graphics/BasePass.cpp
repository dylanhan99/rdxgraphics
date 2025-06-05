#include "BasePass.h"
#include "GLFWWindow/GLFWWindow.h"

bool BasePass::Init(void*)
{
	Terminate();
	
	m_FBO = 0;
	m_TextureBuffer = 0;
	m_ViewportPos = m_BufferPos = { 0, 0 };
	m_ViewportDims = m_BufferDims = GLFWWindow::GetWindowDims();

	return true;
}

bool BasePass::Init(int x, int y, int width, int height)
{
	Terminate();

	m_ViewportPos = { x, y };
	m_ViewportDims = { width, height };

	m_BufferPos = { 0, 0 };
	m_BufferDims = GLFWWindow::GetWindowDims();

	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glGenTextures(1, &m_TextureBuffer);
	glBindTexture(GL_TEXTURE_2D, m_TextureBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_BufferDims.x, m_BufferDims.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureBuffer, 0);

	// Create a depth buffer (needed for depth testing)
	glGenRenderbuffers(1, &m_DepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_BufferDims.x, m_BufferDims.y); // 24-bit depth buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBuffer);

	RX_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FBO init failed somewhere");
	return true;
}

void BasePass::Terminate()
{
	if (m_FBO) glDeleteFramebuffers(1, &m_FBO);
	if (m_TextureBuffer) glDeleteTextures(1, &m_TextureBuffer);
	if (m_DepthBuffer) glDeleteRenderbuffers(1, &m_DepthBuffer);
	m_FBO = m_TextureBuffer = m_DepthBuffer = 0;
}

void BasePass::Draw() const
{
	if (!IsEnabled())
		return;

	for (auto& func : m_UBOBinds)
		if (func) func();

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glBindTexture(GL_TEXTURE_2D, m_TextureBuffer);
	glViewport(m_ViewportPos.x, m_ViewportPos.y, m_ViewportDims.x, m_ViewportDims.y);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glClearColor(m_BackbufferColor.r, m_BackbufferColor.g, m_BackbufferColor.b, m_BackbufferColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawImpl();

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
}