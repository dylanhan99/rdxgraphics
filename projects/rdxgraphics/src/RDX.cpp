#include "RDX.h"

#include "ECS/Components.h"
#include "ECS/EntityManager.h"
#include "ECS/Systems/CollisionSystem.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/TransformSystem.h"
#include "ECS/Systems/BVHSystem.h"
#include "GLFWWindow/GLFWWindow.h"
#include "GSM/SceneManager.h"
#include "GSM/Scenes/CommonLayer.h"
#include "GSM/Scenes/Sandbox.h"
#include "GUI/GUI.h"
#include "Utils/Input.h"

//#include "GSM/Scenes/Assignment1/Assignment1.h"
//#include "GSM/Scenes/Assignment2/Assignment2.h"
#include "GSM/Scenes/Assignment3/Assignment3.h"

void RDX::Run()
{
	Logger::Init();

	bool initOK = true;
	initOK &= GLFWWindow::Init();
	initOK &= EntityManager::Init();
	initOK &= BVHSystem::Init();
	initOK &= RenderSystem::Init();
	initOK &= GUI::Init();
	if (!initOK)
		throw RX_EXCEPTION("System initialization failed");

	SceneManager::RegisterScene<Assignment3>("Assignment 3");
	SceneManager::RegisterScene<Sandbox>("Sandbox");

	//SceneManager::RegisterScene<Assignment2>("Assignment 2");

	//SceneManager::RegisterScene<SphereXSphereScene>("Sphere - Sphere");
	//SceneManager::RegisterScene<AABBXSphereScene>("AABB - Sphere");
	//SceneManager::RegisterScene<AABBXSphereScene>("Sphere - AABB");
	//SceneManager::RegisterScene<AABBXAABBScene>("AABB - AABB");
	//
	//SceneManager::RegisterScene<PointXSphereScene>("Point - Sphere");
	//SceneManager::RegisterScene<PointXAABBScene>("Point - AABB");
	//SceneManager::RegisterScene<PointXTriangleScene>("Point - Triangle");
	//SceneManager::RegisterScene<PointXPlaneScene>("Point - Plane");
	//
	//SceneManager::RegisterScene<RayXSphereScene>("Ray - Sphere");
	//SceneManager::RegisterScene<RayXAABBScene>("Ray - AABB");
	//SceneManager::RegisterScene<RayXTriangleScene>("Ray - Triangle");
	//SceneManager::RegisterScene<RayXPlaneScene>("Ray - Plane");
	//
	//SceneManager::RegisterScene<PlaneXSphereScene>("Plane - Sphere");
	//SceneManager::RegisterScene<PlaneXAABBScene>("Plane - AABB");

	SceneManager::Init<CommonLayer>();

	while (!GLFWWindow::IsWindowShouldClose())
	{
		std::string title = "rdxgraphics [" + std::to_string(GLFWWindow::GetFPSIntervaled()) + "]";
		GLFWWindow::SetWindowTitle(title);

		if (!SceneManager::ResolveScenes())
			GLFWWindow::SetWindowShouldClose();
		GLFWWindow::Update(std::move(
			[&](double deltatime)
			{ 
				float dt = (float)deltatime;
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
