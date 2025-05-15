#include <pch.h>
#include "Sandbox.h"
#include "Utils/Input.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/EntityManager.h"
#include "ECS/Components/Camera.h"

entt::entity mainCameraHandle{};
entt::entity minimapCameraHandle{};

void Sandbox::Load()
{
	{
		auto handle = EntityManager::CreateEntity();
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 1.f, 1.f, 1.f }, glm::vec3{ 1.f }, glm::vec3{ glm::quarter_pi<float>() });
		EntityManager::AddComponent<Model>(handle, Shape::Sphere);
		EntityManager::AddComponent<Collider>(handle, BV::Ray);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f,1.f,0.f });
	}
	{
		auto handle = EntityManager::CreateEntity();
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 1.f }, glm::vec3{ glm::quarter_pi<float>() });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Collider>(handle, BV::Ray);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f,0.f,1.f });
	}
	{
		auto handle = EntityManager::CreateEntity();
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f, 5.f, 0.f }, glm::vec3{ 0.3f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<DirectionalLight>(handle);
	}
	{
		auto& handle = mainCameraHandle;
		handle = EntityManager::CreateEntity();
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
	RenderSystem::SetActiveCamera(mainCameraHandle);
	{
		auto& handle = minimapCameraHandle;
		handle = EntityManager::CreateEntity();
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
	RenderSystem::SetMinimapCamera(minimapCameraHandle);
}

void Sandbox::Start()
{
}

void Sandbox::Update(float dt)
{
	Camera& mainCamera = EntityManager::GetComponent<Camera>(mainCameraHandle);
	Camera& mainmapCamera = EntityManager::GetComponent<Camera>(minimapCameraHandle);

	// This is what is camera view.
	Camera& activeCamera = EntityManager::GetComponent<Camera>(mainCameraHandle);

	if (Input::IsKeyTriggered(RX_KEY_TAB))
	{
		bool& b = activeCamera.IsCameraInUserControl();
		b = !b;

		EventDispatcher<Camera&>::FireEvent(RX_EVENT_CAMERA_USER_TOGGLED, activeCamera);
	}

	if (activeCamera.IsCameraInUserControl())
		activeCamera.Inputs(dt);

	mainCamera.UpdateCameraVectors();
	mainmapCamera.UpdateCameraVectors();

	// Hardcode logic
	{
		{
			auto view = EntityManager::View<Xform>(entt::exclude<Camera, DirectionalLight>);
			for (auto [handle, xform] : view.each())
				xform.GetEulerOrientation() = glm::vec3{ 0.f };//xform.GetEulerOrientation().y += glm::quarter_pi<float>() * dt;
		}
		{
			static float angle = 0.f;
			auto view = EntityManager::View<Xform, DirectionalLight>();
			for (auto [handle, xform, light] : view.each())
			{
				float rate = 0.25f;
				float radius = 5.f;
				angle += glm::two_pi<float>() * rate * dt;
				if (angle > glm::two_pi<float>()) angle = 0.f;

				xform.GetTranslate().x = glm::cos(angle) * radius;
				xform.GetTranslate().y = 0.f;
				xform.GetTranslate().z = glm::sin(angle) * radius;

				light.GetDirection() = glm::normalize(-xform.GetTranslate()); // Look at origin
			}
		}
	}
}
