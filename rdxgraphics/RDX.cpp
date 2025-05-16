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
#include "GSM/SceneManager.h"
#include "GSM/Scenes/Sandbox.h"

void RDX::Run()
{
	Logger::Init();

	bool initOK = true;
	initOK &= GLFWWindow::Init();
	initOK &= RenderSystem::Init();
	initOK &= GUI::Init();
	if (!initOK)
		throw RX_EXCEPTION("System initialization failed");

	SceneManager::Init<
		Sandbox
	>();

	while (!GLFWWindow::IsWindowShouldClose())
	{
		std::string title = "rdxgraphics [" + std::to_string(GLFWWindow::GetFPSIntervaled()) + "]";
		GLFWWindow::SetWindowTitle(title);

		if (!SceneManager::ResolveScenes())
			GLFWWindow::SetWindowShouldClose();
		GLFWWindow::Update(std::move(
			[&](float dt)
			{
				if (Input::IsKeyTriggered(RX_KEY_ESCAPE))
					GLFWWindow::SetWindowShouldClose();

				if (Input::IsKeyTriggered(RX_KEY_F11))
					GLFWWindow::ToggleMinMaxWindow();

				if (Input::IsKeyTriggered(RX_KEY_F5))
					RenderSystem::ReloadShaders();

				if (auto p = SceneManager::GetWorkingScene())
					p->Update(dt);
				
				TransformSystem::Update(dt);
				CollisionSystem::Update(dt);
				GUI::Update(dt);

				// Render
				RenderSystem::Draw();
				GUI::Draw();
				GLFWWindow::MakeContextCurrent(); // Must do this cus of multiviewports
			}
		));
	}

	SceneManager::Terminate();
	GUI::Terminate();
	RenderSystem::Terminate();
	GLFWWindow::Terminate();

	Logger::Terminate();
}
