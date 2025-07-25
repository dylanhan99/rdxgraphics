#include <pch.h>
#include "CommonLayer.h"
#include "Utils/Input.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Components.h"
#include "GUI/GUI.h"

void CommonLayer::StartImpl()
{
	{
		entt::entity handle = m_LightHandle = BaseScene::CreateDefaultEntity<NoDelete>();
		EntityManager::AddComponent<Metadata>(handle, "Directional Light");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f, 5.f, 0.f }, glm::vec3{ 0.3f }, glm::vec3{glm::quarter_pi<float>()});
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<DirectionalLight>(handle);
		//EntityManager::AddComponent<BoundingVolume>(handle, BV::AABB);
	}

	{
		entt::entity handle = m_MainCamera = BaseScene::CreateDefaultEntity<NoDelete>();
		EntityManager::AddComponent<Metadata>(handle, "FPS Cam");
		EntityManager::AddComponent<Xform>(handle,
			glm::vec3{ -30.f, 30.f, 60.f },
			glm::vec3{ 0.2f },
			glm::vec3{ -0.2f, -0.869f, 0.f });
		//EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f,1.f,0.f });
		EntityManager::AddComponent<Camera>(handle,
			Camera::Mode::Perspective,
			glm::vec2{ 16.f, 9.f }, 90.f, glm::vec2{0.1f, 700});
	}

	{
		entt::entity handle = m_MinimapCamera = BaseScene::CreateDefaultEntity<NoDelete>();
		EntityManager::AddComponent<Metadata>(handle, "PiP Cam");
		EntityManager::AddComponent<Xform>(handle,
			glm::vec3{ 0.f, 5.f, 0.f },
			glm::vec3{ 0.2f },
			glm::vec3{ -glm::half_pi<float>() + glm::epsilon<float>(), 0.f, 0.f });
		//EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f,1.f,0.f });
		//EntityManager::AddComponent<Camera>(handle,
		//	Camera::Mode::Perspective,
		//	glm::vec2{ 16.f, 9.f }, 90.f);
		EntityManager::AddComponent<Camera>(handle,
			Camera::Mode::Orthorgonal,
			glm::vec2{ 16.f, 9.f }, 90.f)
			.GetOrthoSize() = 15.f;
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

	if (Input::IsKeyDown(RX_KEY_LEFT_ALT))
	{
		if (Input::IsKeyTriggered(RX_KEY_1))
			cam.SetControlScheme(Camera::ControlScheme::Regular);
		if (Input::IsKeyTriggered(RX_KEY_2))
			cam.SetControlScheme(Camera::ControlScheme::Arcball);
	}

	if (cam.IsCameraInUserControl())
		cam.Inputs(dt, GUI::GetSelectedEntity());

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
		
		const float rate = 0.4f;
		const float radius = 7.f;
		angle += glm::two_pi<float>() * rate * dt;
		if (angle > glm::two_pi<float>()) angle = 0.f;

		auto& pos = xform.GetTranslate();
		pos.x = glm::cos(angle) * radius;
		pos.y = 0.f;
		pos.z = glm::sin(angle) * radius;

		light.GetDirection() = glm::normalize(-xform.GetTranslate()); // Look at origin
	}

	//EntityManager::AddComponent<BoundingVolume::DirtyXform>(m_LightHandle);
}
