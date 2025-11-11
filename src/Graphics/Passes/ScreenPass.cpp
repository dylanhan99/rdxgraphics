#include "ScreenPass.h"
#include "ECS/EntityManager.h"
#include "ECS/Systems/RenderSystem.h"
#include "Graphics/Passes/Passes.h"

void ScreenPass::DrawImpl() const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	RenderSystem::GetInstance().m_FBOShader.Bind();

	auto& passes = RenderSystem::GetRenderPasses();
	for (int i = 0; i < passes.size() - 1; ++i) // size - 1 hardcode, assuming that screenpass is the LAST registered one.
	{
		int glTextureSlot = GL_TEXTURE0 + i;
		std::shared_ptr<BasePass> pass = passes[i];

		// Only have 32 texture slots don't mess up lmao
		RX_ASSERT(glTextureSlot <= GL_TEXTURE31);

		glActiveTexture(glTextureSlot);
		glBindTexture(GL_TEXTURE_2D, pass->GetTextureBuffer());
		RenderSystem::GetInstance().m_FBOShader.SetUniform1i(pass->GetHandleTexName().c_str(), i);
		RenderSystem::GetInstance().m_FBOShader.SetUniform1i(pass->GetHasHandleName().c_str(), pass->IsEnabled());
	}

	RenderSystem::GetInstance().m_FBOObject.Bind();
	RenderSystem::GetInstance().m_FBOObject.Draw(1);
}