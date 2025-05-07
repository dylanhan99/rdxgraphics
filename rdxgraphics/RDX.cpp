#include <pch.h>
#include "RDX.h"

#include "Utils/Input.h"
#include "GLFWWindow/GLFWWindow.h"
#include "GLFWWindow/FramerateController.h"
#include "Rendering/RenderSystem.h"
#include "Transformation/TransformSystem.h"
#include "Collision/CollisionSystem.h"
#include "Entity/EntityManager.h"
#include "GUI/GUI.h"

#include "Rendering/Camera.h"

Camera mainCamera{ { -3.f,3.f,3.f }, { -0.7f,-0.7f,0.f }, { 16.f, 9.f }, 90.f };

void RDX::Run()
{
	Logger::Init();
	FramerateController::Init(30);

	bool initOK = true;
	initOK &= GLFWWindow::Init();
	initOK &= RenderSystem::Init();
	initOK &= GUI::Init();
	if (!initOK)
		throw RX_EXCEPTION("System initialization failed");

	auto& entities = EntityManager::GetEntities();
	{
		Entity newEnt{};
		newEnt.SetCollider<Ray>();
		entities.emplace_back(std::move(newEnt));
	}
	{
		Entity newEnt{};
		newEnt.SetCollider<Sphere>();
		entities.emplace_back(std::move(newEnt));
	}

	while (!GLFWWindow::IsWindowShouldClose())
	{
		FramerateController::StartGameLoop();
		std::string title = "rdxgraphics [" + std::to_string(FramerateController::GetFPSIntervaled()) + "]";
		GLFWWindow::SetWindowTitle(title);
		FramerateController::Update(std::move(
			[&](float dt)
			{
				GLFWWindow::StartFrame();

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

				GLFWWindow::EndFrame();
			}
		));
	}

	GUI::Terminate();
	RenderSystem::Terminate();
	GLFWWindow::Terminate();

	Logger::Terminate();
}
