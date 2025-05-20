#include <pch.h>
#include "RDX.h"

#include "ECS/Components.h"
#include "ECS/EntityManager.h"
#include "ECS/Systems/CollisionSystem.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/TransformSystem.h"
#include "GLFWWindow/GLFWWindow.h"
#include "GSM/SceneManager.h"
#include "GSM/Scenes/CommonLayer.h"
#include "GSM/Scenes/Sandbox.h"
#include "GUI/GUI.h"
#include "Utils/Input.h"

void RDX::Run()
{
	Logger::Init();

	bool initOK = true;
	initOK &= GLFWWindow::Init();
	initOK &= RenderSystem::Init();
	initOK &= GUI::Init();
	if (!initOK)
		throw RX_EXCEPTION("System initialization failed");

#define _RX_REG_SCN(Klass, ...)\
	SceneManager::RegisterScene<Klass>(#Klass, ##__VA_ARGS__)
	_RX_REG_SCN(Sandbox);
#undef _RX_REG_SCN
	SceneManager::Init<CommonLayer>();

	while (!GLFWWindow::IsWindowShouldClose())
	{
		std::string title = "rdxgraphics [" + std::to_string(GLFWWindow::GetFPSIntervaled()) + "]";
		GLFWWindow::SetWindowTitle(title);

		if (!SceneManager::ResolveScenes())
			GLFWWindow::SetWindowShouldClose();
		GLFWWindow::Update(std::move(
			[&](float dt)
			{
				if (!GLFWWindow::IsIconified())
				{
					if (Input::IsKeyTriggered(RX_KEY_ESCAPE))
					GLFWWindow::SetWindowShouldClose();

					if (Input::IsKeyTriggered(RX_KEY_F11))
						GLFWWindow::ToggleMinMaxWindow();

					if (Input::IsKeyTriggered(RX_KEY_F5))
						RenderSystem::ReloadShaders();

					GUI::Update(dt);

					auto camView = EntityManager::View<Camera>();
					for (auto [handle, cam] : camView.each())
					{
						cam.UpdateCameraVectors();
					}
				}

				SceneManager::Update(dt);
				TransformSystem::Update(dt);
				CollisionSystem::Update(dt);

				// Render
				if (!GLFWWindow::IsIconified())
				{
					RenderSystem::Draw();
					GUI::Draw();
					GLFWWindow::MakeContextCurrent(); // Must do this cus of multiviewports
				}
			}
		));
	}

	SceneManager::Terminate();
	GUI::Terminate();
	RenderSystem::Terminate();
	GLFWWindow::Terminate();

	Logger::Terminate();
}
