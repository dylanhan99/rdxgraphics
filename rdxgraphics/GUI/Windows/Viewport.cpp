#include <pch.h>
#include "Viewport.h"
#include "ECS/Systems/RenderSystem.h"

void Viewport::UpdateImpl(float dt)
{
	auto& pass = RenderSystem::GetScreenPass();
	GLuint const frameHandle = pass.m_TextureBuffer;
	ImGui::Image(frameHandle, 
		ImVec2{ (float)pass.m_BufferDims.x, (float)pass.m_BufferDims.y }, 
		{ 0.f, 1.f }, { 1.f, 0.f }); // It's flipped vertically
}