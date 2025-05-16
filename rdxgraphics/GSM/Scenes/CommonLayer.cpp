#include <pch.h>
#include "CommonLayer.h"
#include "Utils/Input.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Components.h"

void CommonLayer::Load()
{
	m_MainCamera = BaseScene::CreateEntity<NoDelete>();
	m_MinimapCamera = BaseScene::CreateEntity<NoDelete>();
}

void CommonLayer::Start()
{
	m_ActiveCamera = m_MainCamera;
	RenderSystem::SetActiveCamera(m_ActiveCamera);
	RenderSystem::SetMinimapCamera(m_MinimapCamera);

	{
		entt::entity handle = m_MainCamera;
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f,1.f,0.f });
		EntityManager::AddComponent<Camera>(handle,
			Camera::Mode::Perspective,
			glm::vec3{},
			glm::vec2{ 16.f, 9.f }, 90.f);
		EntityManager::AddComponent<Xform>(handle,
			glm::vec3{ -3.f, 3.f, 3.f },
			glm::vec3{ 0.2f },
			glm::vec3{ -0.7f, -0.7f, 0.f });
	}
	{
		entt::entity handle = m_MinimapCamera;
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f,1.f,0.f });
		EntityManager::AddComponent<Camera>(handle,
			Camera::Mode::Orthorgonal,
			glm::vec3{},
			glm::vec2{ 16.f, 9.f }, 90.f);
		EntityManager::AddComponent<Xform>(handle,
			glm::vec3{ 0.f, 5.f, 0.f },
			glm::vec3{ 0.2f },
			glm::vec3{ -glm::half_pi<float>() + glm::epsilon<float>(), 0.f, 0.f });
	}
}

void CommonLayer::Update(float dt)
{
	// This is what is camera view.
	Camera& cam = EntityManager::GetComponent<Camera>(RenderSystem::GetActiveCamera());

	if (Input::IsKeyTriggered(RX_KEY_TAB))
	{
		bool& b = cam.IsCameraInUserControl();
		b = !b;

		EventDispatcher<Camera&>::FireEvent(RX_EVENT_CAMERA_USER_TOGGLED, cam);
	}

	if (cam.IsCameraInUserControl())
		cam.Inputs(dt);
}
