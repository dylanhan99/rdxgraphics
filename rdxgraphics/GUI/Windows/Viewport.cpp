#include <pch.h>
#include "Viewport.h"
#include "ECS/Systems/RenderSystem.h"

void Viewport::UpdateImpl(float dt)
{
	auto& pass = RenderSystem::GetScreenPass();
	GLuint const frameHandle = pass.GetTextureBuffer();
	ImGui::Image(frameHandle, 
		ImVec2{ (float)pass.GetBufferDims().x, (float)pass.GetBufferDims().y},
		{ 0.f, 1.f }, { 1.f, 0.f }); // It's flipped vertically
}