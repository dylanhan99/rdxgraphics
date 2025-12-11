#include "NaiveBasePass.h"
#include "ServiceLayer.h"
#include "Event/Events/Events.h"

using namespace rdx;

bool NaiveBasePass::Init()
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

	if (!m_Camera)
	{
		m_Camera = ServiceLayer::RenderingSystem()->GetCurrentCamera(); // A default

		// And assuming that no m_Camera means it's a game's pass, we'd need to respond to the game's Camera Chagne Event
		ServiceLayer::InstantEventService()->Subscribe<CameraChangeEvent>(
			[this](CameraChangeEvent const& e)
			{
				m_Camera = e.pCamera;
			});
	}

	return InitImpl();
}

bool NaiveBasePass::Terminate()
{
	TerminateImpl();

	if (m_FBO) glDeleteFramebuffers(1, &m_FBO);
	if (m_TextureBuffer) glDeleteTextures(1, &m_TextureBuffer);
	if (m_DepthBuffer) glDeleteRenderbuffers(1, &m_DepthBuffer);
	m_FBO = m_TextureBuffer = m_DepthBuffer = 0;

	return true;
}

void NaiveBasePass::Draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	DrawImpl();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}