#include <pch.h>
#include "ScreenPass.h"
#include "ECS/EntityManager.h"
#include "ECS/Systems/RenderSystem.h"
#include "Graphics/Passes/Passes.h"

bool hasDefault = true;
bool hasWireframe = true;
bool hasMinimap = true;

void ScreenPass::DrawImpl() const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	RenderSystem::GetInstance().m_FBOShader.Bind();

	auto& basePass = RenderSystem::GetRenderPass<ModelPass>();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, basePass.GetTextureBuffer());
	RenderSystem::GetInstance().m_FBOShader.SetUniform1i("uBaseTex", 0);
	RenderSystem::GetInstance().m_FBOShader.SetUniform1i("uHasBaseTex", hasDefault);
	
	auto& wireframePass = RenderSystem::GetRenderPass<ColliderWireframePass>();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, wireframePass.GetTextureBuffer());
	RenderSystem::GetInstance().m_FBOShader.SetUniform1i("uWireframeTex", 1);
	RenderSystem::GetInstance().m_FBOShader.SetUniform1i("uHasWireframeTex", hasWireframe);

	//auto& wireframePass = RenderSystem::GetRenderPass<ColliderWireframePass>();
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, minimapPass.GetTextureBuffer());
	//g.m_FBOShader.SetUniform1i("uMinimapTex", 2);
	RenderSystem::GetInstance().m_FBOShader.SetUniform1i("uHasMinimapTex", false/*hasMinimap*/);

	RenderSystem::GetInstance().m_FBOObject.Bind();
	RenderSystem::GetInstance().m_FBOObject.Draw(1);
}