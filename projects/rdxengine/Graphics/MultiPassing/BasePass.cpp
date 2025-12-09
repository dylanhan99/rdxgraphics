#include "BasePass.h"

using namespace rdx;

bool BasePass::Init()
{
	glCreateFramebuffers(1, &m_FBO);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureBuffer);
	glTextureStorage2D(m_TextureBuffer, 1, GL_RGBA8, m_BufferDims.x, m_BufferDims.y);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glNamedFramebufferTexture(m_FBO, GL_COLOR_ATTACHMENT0, m_TextureBuffer, 0);

	glCreateRenderbuffers(1, &m_DepthBuffer);
	glNamedRenderbufferStorage(m_DepthBuffer, GL_DEPTH_COMPONENT24, m_BufferDims.x, m_BufferDims.y);
	glNamedFramebufferRenderbuffer(m_FBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBuffer);

	RX_ASSERT(glCheckNamedFramebufferStatus(m_FBO, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	return true;
}

bool BasePass::Terminate()
{
	if (m_FBO) glDeleteFramebuffers(1, &m_FBO);
	if (m_TextureBuffer) glDeleteTextures(1, &m_TextureBuffer);
	if (m_DepthBuffer) glDeleteRenderbuffers(1, &m_DepthBuffer);
	m_FBO = m_TextureBuffer = m_DepthBuffer = 0;

	return true;
}

void BasePass::Draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	DrawImpl();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}