#include <pch.h>
#include "RDX.h"

#include "Utils/Input.h"
#include "GLFWWindow/GLFWWindow.h"
#include "ECS/Systems/RenderSystem.h"
#include "Transformation/TransformSystem.h"
#include "ECS/Systems/CollisionSystem.h"
#include "ECS/EntityManager.h"
#include "GUI/GUI.h"

#include "ECS/Components/Camera.h"

//Camera mainCamera{ { -3.f,3.f,3.f }, { -0.7f,-0.7f,0.f }, { 16.f, 9.f }, 90.f };

void RDX::Run()
{
	Logger::Init();

	bool initOK = true;
	initOK &= GLFWWindow::Init();
	initOK &= RenderSystem::Init();
	initOK &= GUI::Init();
	if (!initOK)
		throw RX_EXCEPTION("System initialization failed");

	{
		auto handle = EntityManager::CreateEntity<Xform>();
		EntityManager::AddComponent<Model>(handle, Shape::Quad);
		EntityManager::AddComponent<Collider>(handle, BV::AABB);
	}
	{
		auto handle = EntityManager::CreateEntity();
		EntityManager::AddComponent<Xform>(handle, glm::vec3{1.f, 1.f, 1.f});
		EntityManager::AddComponent<Model>(handle, Shape::Quad);
		EntityManager::AddComponent<Collider>(handle, BV::Sphere);
	}
	entt::entity mainCameraHandle{};
	{
		auto& handle = mainCameraHandle;
		handle = EntityManager::CreateEntity();
		EntityManager::AddComponent<Camera>(handle,
			glm::vec3{ -3.f, 3.f, 3.f }, 
			glm::vec3{ -0.7f, -0.7f, 0.f }, 
			glm::vec2{ 16.f, 9.f }, 90.f);
	}
	RenderSystem::SetActiveCamera(mainCameraHandle);
	Camera& mainCamera = EntityManager::GetComponent<Camera>(mainCameraHandle);

	while (!GLFWWindow::IsWindowShouldClose())
	{
		std::string title = "rdxgraphics [" + std::to_string(GLFWWindow::GetFPSIntervaled()) + "]";
		GLFWWindow::SetWindowTitle(title);
		GLFWWindow::Update(std::move(
			[&](float dt)
			{
				if (Input::IsKeyTriggered(RX_KEY_ESCAPE))
					GLFWWindow::SetWindowShouldClose();

				if (Input::IsKeyTriggered(RX_KEY_F11))
					GLFWWindow::ToggleMinMaxWindow();

				if (Input::IsKeyTriggered(RX_KEY_F5))
					RenderSystem::ReloadShaders();

				if (Input::IsKeyTriggered(RX_KEY_TAB))
				{
					bool& b = mainCamera.IsCameraInUserControl();
					b = !b;

					EventDispatcher<Camera&>::FireEvent(RX_EVENT_CAMERA_USER_TOGGLED, mainCamera);
				}

				if (mainCamera.IsCameraInUserControl())
					mainCamera.Inputs(dt);

				mainCamera.UpdateCameraVectors();

				TransformSystem::Update(dt);
				CollisionSystem::Update(dt);
				GUI::Update(dt);

				// Render
				{
					RenderSystem::Draw();
					GUI::Draw();
				}
			}
		));
	}

	GUI::Terminate();
	RenderSystem::Terminate();
	GLFWWindow::Terminate();

	Logger::Terminate();
}
