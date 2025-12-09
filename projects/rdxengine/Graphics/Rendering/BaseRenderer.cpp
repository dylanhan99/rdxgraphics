#include "BaseRenderer.h"
#include "ECS/Components/CameraComponent.h"

using namespace rdx;

void BaseRenderer::Draw() 
{ 
	// This is hardcoded
	if (m_CurrentCamera)
		m_CurrentCamera->UpdateCameraVectors(glm::vec3{ 0.f, 0.f, 5.f }, glm::vec3{ 0.f });

	DrawImpl(); 
}

void BaseRenderer::SetDepthTest(bool flag)
{
	
}