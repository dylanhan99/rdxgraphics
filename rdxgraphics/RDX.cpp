#include <pch.h>
#include "RDX.h"

#include "Utils/Input.h"
#include "GLFWWindow/GLFWWindow.h"
#include "Rendering/RenderSystem.h"
#include "Transformation/TransformSystem.h"
#include "Collision/CollisionSystem.h"
#include "Entity/EntityManager.h"

#include "Rendering/Camera.h"
#include "Utils/FramerateController.h"

Camera mainCamera{ { -3.f,3.f,3.f }, { -0.7f,-0.7f,0.f }, { 16.f, 9.f }, 90.f };
extern float move;
extern int renderOption;

void RDX::Run()
{
	Logger::Init();
	FramerateController::Init(30);

	bool initOK = true;
	initOK &= GLFWWindow::Init();
	initOK &= RenderSystem::Init();

	GLFWwindow* pWindow = GLFWWindow::GetWindowPointer();
	{
		// Initialize ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(pWindow, true);
		ImGui_ImplOpenGL3_Init("#version 450");  // Your GLSL version
	}

	if (!initOK)
		throw RX_EXCEPTION("System initialization failed");

	auto& entities = EntityManager::GetEntities();
	{
		Entity newEnt{};
		auto& ccc = newEnt.GetColliderDetails();
		ccc.BVType = BV::Ray;
		ccc.pBV = std::make_shared<Ray>();
		entities.emplace_back(std::move(newEnt));
	}
	{
		Entity newEnt{};
		auto& ccc = newEnt.GetColliderDetails();
		ccc.BVType = BV::Sphere;
		ccc.pBV = std::make_shared<Sphere>();
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

				if (Input::IsKeyTriggered(GLFW_KEY_ESCAPE))
					GLFWWindow::SetWindowShouldClose();

				if (Input::IsKeyTriggered(GLFW_KEY_F11))
					GLFWWindow::ToggleMinMaxWindow();

				if (Input::IsKeyTriggered(GLFW_KEY_F5))
					RenderSystem::ReloadShaders();

				if (Input::IsKeyTriggered(GLFW_KEY_TAB))
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

				{ // ImGui update
					ImGui_ImplOpenGL3_NewFrame();
					ImGui_ImplGlfw_NewFrame();
					ImGui::NewFrame();
					//ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

					ImGui::Begin("dxd", nullptr, 0);
					{
						//auto& eee = entities[0];
						//auto& aaa = eee.GetColliderDetails();
						//auto asd = std::dynamic_pointer_cast<Ray>(aaa.pBV);
						//ImGui::DragFloat3("colPos", glm::value_ptr(aaa.pBV->GetPosition()));
						//ImGui::DragFloat3("dir", glm::value_ptr(asd->GetOrientation()));
						////ImGui::DragFloat("asd", &asd->ang);

						{
							auto& eee = entities[0];
							ImGui::DragFloat3("pos1", &eee.GetModelDetails().Translate[0], 0.1f);
							if (Ray* pp = (Ray*)eee.GetColliderDetails().pBV.get())
							{
								glm::vec3 dir = pp->GetDirection();
								ImGui::DragFloat3("eul1", &pp->GetOrientation()[0], 0.1f);
								ImGui::DragFloat3("dir1", &dir[0], 0.1f);
							}
						}
						{
							auto& eee = entities[1];
							ImGui::DragFloat3("pos2", &eee.GetModelDetails().Translate[0], 0.1f);
							//if (Plane* pp = (Plane*)eee.GetColliderDetails().pBV.get())
							//{
							//	ImGui::DragFloat3("eul2", &pp->GetOrientation()[0], 0.1f);
							//}
						}
						//auto& aaa = eee.GetColliderDetails();
						//auto asd = std::dynamic_pointer_cast<Plane>(aaa.pBV);
						//ImGui::DragFloat3("colPos", glm::value_ptr(aaa.pBV->GetPosition()));
						//ImGui::DragFloat3("dir", glm::value_ptr(asd->GetOrientation()));
						//ImGui::DragFloat("asd", &asd->ang);

						ImGui::Separator();

						ImGui::DragFloat("move", &move);
						ImGui::RadioButton("Mesh", &renderOption, 0); ImGui::SameLine();
						ImGui::RadioButton("Wire", &renderOption, 1); ImGui::SameLine();
						ImGui::RadioButton("Both", &renderOption, 2);

						static int s_FPSIndex = 0; // Defaults at 30
						static std::array<const char*, 6> s_FPSs{ "30", "60", "120", "144", "240", "No Lim." };
						static std::array<uint32_t, 6> s_FPSsNum{ 30, 60, 120, 144, 240, std::numeric_limits<uint32_t>::max() };
						ImGui::SeparatorText("Window");
						if (ImGui::SliderInt("FPS", &s_FPSIndex, 0, static_cast<int>(s_FPSs.size() - 1), s_FPSs[s_FPSIndex]))
							FramerateController::SetTargetFPS(s_FPSsNum[s_FPSIndex]);
						if (ImGui::Checkbox("VSync", &GLFWWindow::IsVSync()))
							GLFWWindow::SetIsVSync(GLFWWindow::IsVSync());

						ImGui::SeparatorText("Graphics");
						ImGui::ColorEdit3("Back Buffer Color", glm::value_ptr(RenderSystem::GetBackBufferColor()));

						ImGui::SeparatorText("Camera Settings");
						{
							Camera& cam = mainCamera;
							glm::vec3 camPos{}, camFace{};
							camPos = cam.GetPosition();
							camFace = cam.GetEulerOrientation();

							ImGui::Text("Cam [Pos]|X:% -4.1f |Y:% -4.1f |Z:% -4.1f", camPos.x, camPos.y, camPos.z);
							ImGui::Text("    [Dir]|X:% -4.1f |Y:% -4.1f |Z:% -4.1f", camFace.x, camFace.y, camFace.z);
							if (ImGui::Checkbox("CameraToggled", &cam.IsCameraInUserControl()))
								EventDispatcher<Camera&>::FireEvent(RX_EVENT_CAMERA_USER_TOGGLED, cam);

							if (ImGui::TreeNode("Advanced"))
							{
								ImGui::DragFloat("FOV", &cam.GetFOV(), 1.f, 0.f, 105.f, "%.0f");
								ImGui::DragFloat2("Near/Far", glm::value_ptr(cam.GetClipPlanes()), 1.f, 0.f, std::numeric_limits<float>::max(), "%.0f");
								ImGui::DragFloat("Camera Speed", &cam.GetMovementSpeed(), 0.1f, 0.f, std::numeric_limits<float>::max(), "%.1f");
								ImGui::DragFloat("Pitch Speed", &cam.GetPitchSpeed(), 0.1f, 0.f, std::numeric_limits<float>::max(), "%.1f");
								ImGui::DragFloat("Yaw Speed", &cam.GetYawSpeed(), 0.1f, 0.f, std::numeric_limits<float>::max(), "%.1f");

								ImGui::TreePop();
							}
						}
					}
					ImGui::End();
					ImGui::Render();
				}

				// Render
				{
					RenderSystem::Update(dt);

					glDisable(GL_DEPTH_TEST);
					ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
				}

				GLFWWindow::EndFrame();
			}
		));
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	RenderSystem::Terminate();
	GLFWWindow::Terminate();

	Logger::Terminate();
}
