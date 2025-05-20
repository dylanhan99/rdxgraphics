#include <pch.h>
#include "GUI.h"

#include "Utils/Input.h"
#include "GLFWWindow/GLFWWindow.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/TransformSystem.h"
#include "ECS/Systems/CollisionSystem.h"
#include "ECS/EntityManager.h"
#include "ECS/Components/Camera.h"

#include "Windows/EntityHierarchy.h"
#include "Windows/Inspector.h"
#include "Windows/Viewport.h"

RX_SINGLETON_EXPLICIT(GUI);
extern float move;
extern int renderOption;

void BaseGUIWindow::Update(float dt)
{
	if (ImGui::Begin(m_Name.c_str(), nullptr, m_Flags))
		UpdateImpl(dt);
	ImGui::End();
}

bool GUI::Init()
{
	// Initialize ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		  // Enable detatchable imgui windows
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(GLFWWindow::GetWindowPointer(), true);
	ImGui_ImplOpenGL3_Init("#version 460");

	g.m_GUIWindows.emplace_back(std::make_unique<EntityHierarchy>("Entity Hierarchy", 0));
	g.m_GUIWindows.emplace_back(std::make_unique<Inspector>("Inspector", 0));
	g.m_GUIWindows.emplace_back(std::make_unique<Viewport>("Viewport", 0));

	return true;
}

void GUI::Terminate()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GUI::Update(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

	for (auto& pGUIWin : g.m_GUIWindows)
		pGUIWin->Update(dt);

	//ImGui::Begin("dxd", nullptr, 0);
	//{
	//	auto view = EntityManager::GetInstance().m_Registry.view<Xform, Model>();
	//	std::string name = "Ent #";
	//	for (auto [handle, xform, model] : view.each())
	//	{
	//		std::string id = name + std::to_string((uint32_t)handle);
	//		if (ImGui::TreeNode(id.c_str()))
	//		{
	//			ImGui::SeparatorText("Xform");
	//			ImGui::DragFloat3(("Pos##" + id).c_str(), glm::value_ptr(xform.GetTranslate()), 0.01f);
	//			ImGui::DragFloat3(("Scl##" + id).c_str(), glm::value_ptr(xform.GetScale()), 0.01f);
	//			ImGui::DragFloat3(("Rot##" + id).c_str(), glm::value_ptr(xform.GetEulerOrientation()), 0.01f);
	//			ImGui::TreePop();
	//			ImGui::SeparatorText("");
	//		}
	//	}
	//
	//	//auto& eee = entities[0];
	//	//auto& aaa = eee.GetColliderDetails();
	//	//auto asd = std::dynamic_pointer_cast<Ray>(aaa.pBV);
	//	//ImGui::DragFloat3("colPos", glm::value_ptr(aaa.pBV->GetPosition()));
	//	//ImGui::DragFloat3("dir", glm::value_ptr(asd->GetOrientation()));
	//	////ImGui::DragFloat("asd", &asd->ang);
	//
	//	//{
	//	//	auto& eee = entities[0];
	//	//	ImGui::DragFloat3("pos1", &eee.GetModelDetails().Translate[0], 0.1f);
	//	//	if (Ray* pp = (Ray*)eee.GetColliderDetails().pBV.get())
	//	//	{
	//	//		glm::vec3 dir = pp->GetDirection();
	//	//		ImGui::DragFloat3("eul1", &pp->GetOrientation()[0], 0.1f);
	//	//		ImGui::DragFloat3("dir1", &dir[0], 0.1f);
	//	//	}
	//	//}
	//	//{
	//	//	auto& eee = entities[1];
	//	//	ImGui::DragFloat3("pos2", &eee.GetModelDetails().Translate[0], 0.1f);
	//	//	//if (Plane* pp = (Plane*)eee.GetColliderDetails().pBV.get())
	//	//	//{
	//	//	//	ImGui::DragFloat3("eul2", &pp->GetOrientation()[0], 0.1f);
	//	//	//}
	//	//}
	//	//auto& aaa = eee.GetColliderDetails();
	//	//auto asd = std::dynamic_pointer_cast<Plane>(aaa.pBV);
	//	//ImGui::DragFloat3("colPos", glm::value_ptr(aaa.pBV->GetPosition()));
	//	//ImGui::DragFloat3("dir", glm::value_ptr(asd->GetOrientation()));
	//	//ImGui::DragFloat("asd", &asd->ang);
	//
	//	ImGui::Separator();
	//
	//	ImGui::DragFloat("move", &move);
	//	ImGui::RadioButton("Mesh", &renderOption, 0); ImGui::SameLine();
	//	ImGui::RadioButton("Wire", &renderOption, 1); ImGui::SameLine();
	//	ImGui::RadioButton("Both", &renderOption, 2);
	//
	//	static int s_FPSIndex = 0; // Defaults at 30
	//	static std::array<const char*, 6> s_FPSs{ "30", "60", "120", "144", "240", "No Lim." };
	//	static std::array<uint32_t, 6> s_FPSsNum{ 30, 60, 120, 144, 240, std::numeric_limits<uint32_t>::max() };
	//	ImGui::SeparatorText("Window");
	//	if (ImGui::SliderInt("FPS", &s_FPSIndex, 0, static_cast<int>(s_FPSs.size() - 1), s_FPSs[s_FPSIndex]))
	//		GLFWWindow::SetTargetFPS(s_FPSsNum[s_FPSIndex]);
	//	if (ImGui::Checkbox("VSync", &GLFWWindow::IsVSync()))
	//		GLFWWindow::SetIsVSync(GLFWWindow::IsVSync());
	//
	//	ImGui::SeparatorText("Graphics");
	//	ImGui::ColorEdit3("Back Buffer Color", glm::value_ptr(RenderSystem::GetBackBufferColor()));
	//
	//	ImGui::SeparatorText("Camera Settings");
	//	{
	//		auto camcam = [](entt::entity camHandle, std::string label)
	//			{
	//				if (ImGui::TreeNode(label.c_str()))
	//				{
	//					Xform& xform = EntityManager::GetComponent<Xform>(camHandle);
	//					Camera& cam = EntityManager::GetComponent<Camera>(camHandle);
	//					glm::vec3 camPos = xform.GetTranslate();
	//					glm::vec3 camFace = xform.GetEulerOrientation();
	//
	//					ImGui::Text("Cam [Pos]|X:% -4.1f |Y:% -4.1f |Z:% -4.1f", camPos.x, camPos.y, camPos.z);
	//					ImGui::Text("    [Dir]|X:% -4.1f |Y:% -4.1f |Z:% -4.1f", camFace.x, camFace.y, camFace.z);
	//					if (ImGui::Checkbox("CameraToggled", &cam.IsCameraInUserControl()))
	//						EventDispatcher<Camera&>::FireEvent(RX_EVENT_CAMERA_USER_TOGGLED, cam);
	//
	//					if (ImGui::TreeNode("Advanced"))
	//					{
	//						ImGui::DragFloat("FOV", &cam.GetFOV(), 1.f, 0.f, 105.f, "%.0f");
	//						ImGui::DragFloat2("Near/Far", glm::value_ptr(cam.GetClipPlanes()), 1.f, 0.f, std::numeric_limits<float>::max(), "%.0f");
	//						ImGui::DragFloat("Camera Speed", &cam.GetMovementSpeed(), 0.1f, 0.f, std::numeric_limits<float>::max(), "%.1f");
	//						ImGui::DragFloat("Pitch Speed", &cam.GetPitchSpeed(), 0.1f, 0.f, std::numeric_limits<float>::max(), "%.1f");
	//						ImGui::DragFloat("Yaw Speed", &cam.GetYawSpeed(), 0.1f, 0.f, std::numeric_limits<float>::max(), "%.1f");
	//
	//						ImGui::TreePop();
	//					}
	//
	//					ImGui::TreePop();
	//				}
	//			};
	//
	//		camcam(RenderSystem::GetActiveCamera(), "Main cam");
	//		camcam(RenderSystem::GetMinimapCamera(), "Minimap cam");
	//	}
	//}
	//ImGui::End();
}

void GUI::Draw()
{
	glDisable(GL_DEPTH_TEST);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) 
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}
