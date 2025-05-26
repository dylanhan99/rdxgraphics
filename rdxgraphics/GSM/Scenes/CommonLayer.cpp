#include <pch.h>
#include "CommonLayer.h"
#include "Utils/Input.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Components.h"

void CommonLayer::StartImpl()
{
	{
		entt::entity handle = m_LightHandle = BaseScene::CreateDefaultEntity<NoDelete>();
		EntityManager::AddComponent<Metadata>(handle, "Directional Light");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f, 5.f, 0.f }, glm::vec3{ 0.3f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<DirectionalLight>(handle);
	}

	{
		entt::entity handle = m_MainCamera = BaseScene::CreateDefaultEntity<NoDelete>();
		EntityManager::AddComponent<Metadata>(handle, "FPS Cam");
		EntityManager::AddComponent<Xform>(handle,
			glm::vec3{ -3.f, 3.f, 3.f },
			glm::vec3{ 0.2f },
			glm::vec3{ -0.7f, -0.7f, 0.f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f,1.f,0.f });
		EntityManager::AddComponent<Camera>(handle,
			Camera::Mode::Perspective,
			glm::vec2{ 16.f, 9.f }, 90.f);
	}

	{
		entt::entity handle = m_MinimapCamera = BaseScene::CreateDefaultEntity<NoDelete>();
		EntityManager::AddComponent<Metadata>(handle, "PiP Cam");
		EntityManager::AddComponent<Xform>(handle,
			glm::vec3{ 0.f, 5.f, 0.f },
			glm::vec3{ 0.2f },
			glm::vec3{ -glm::half_pi<float>() + glm::epsilon<float>(), 0.f, 0.f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f,1.f,0.f });
		EntityManager::AddComponent<Camera>(handle,
			Camera::Mode::Orthorgonal,
			glm::vec2{ 16.f, 9.f }, 90.f);
	}

	m_ActiveCamera = m_MainCamera;
	RenderSystem::SetActiveCamera(m_ActiveCamera);
	RenderSystem::SetMinimapCamera(m_MinimapCamera);
}

void CommonLayer::UpdateImpl(float dt)
{
	Camera& cam = EntityManager::GetComponent<Camera>(RenderSystem::GetActiveCamera());

	if (Input::IsKeyTriggered(RX_KEY_TAB))
	{
		bool& b = cam.IsCameraInUserControl();
		b = !b;

		EventDispatcher<Camera&>::FireEvent(RX_EVENT_CAMERA_USER_TOGGLED, cam);
	}

	if (cam.IsCameraInUserControl())
		cam.Inputs(dt);

	// PiP following FPS's X/Z
	if (EntityManager::HasComponent<Xform::Dirty>(cam.GetEntityHandle()))
	{
		auto [xform, pip] = EntityManager::GetComponent<Xform, Camera>(RenderSystem::GetMinimapCamera());
		auto& pos = xform.GetTranslate();
		glm::vec3 followPos = cam.GetPosition();
		pos.x = followPos.x;
		pos.z = followPos.z;
	}

	// Directional light moving in a circle
	{
		static float angle = 0.f;
		auto& xform = EntityManager::GetComponent<Xform>(m_LightHandle);
		auto& light = EntityManager::GetComponent<DirectionalLight>(m_LightHandle);
		
		const float rate = 0.25f;
		const float radius = 5.f;
		angle += glm::two_pi<float>() * rate * dt;
		if (angle > glm::two_pi<float>()) angle = 0.f;

		auto& pos = xform.GetTranslate();
		pos.x = glm::cos(angle) * radius;
		pos.y = 0.f;
		pos.z = glm::sin(angle) * radius;

		light.GetDirection() = glm::normalize(-xform.GetTranslate()); // Look at origin
	}
}
