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
#include "Windows/Settings.h"

RX_SINGLETON_EXPLICIT(GUI);
extern float move;
extern int renderOption;

void BaseGUIWindow::Update(float dt)
{
	//ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always); // or ImGuiCond_FirstUseEver
	//ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);
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
	io.IniFilename = nullptr; // Disable .ini persistence
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(GLFWWindow::GetWindowPointer(), true);
	ImGui_ImplOpenGL3_Init("#version 460");

	g.m_GUIWindows.emplace_back(std::make_unique<EntityHierarchy>("Hierarchy", 0));
	g.m_GUIWindows.emplace_back(std::make_unique<Inspector>("Inspector", 0));
	g.m_GUIWindows.emplace_back(std::make_unique<Viewport>("Viewport", ImGuiWindowFlags_NoScrollbar));
	g.m_GUIWindows.emplace_back(std::make_unique<Settings>("Settings", 0));

	g.m_DockID = static_cast<uint32_t>(Rxuid{ "RX_DOCKSPACE_UID" });

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

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::DockSpaceOverViewport(g.m_DockID, viewport);
	
	static bool dockLoaded{ false };
	if (!dockLoaded) // lazy init dock
	{
		dockLoaded = true;
		ImGuiID dockID = g.m_DockID;

		// Clear any existing layout
		ImGui::DockBuilderRemoveNode(dockID);
		ImGui::DockBuilderAddNode(dockID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockID, viewport->Size);

		// Split the dockspace
		ImGuiID mainDockID{ dockID }, leftDockID{}, rightDockID{}, downDockID{};
		ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Left, 0.2f, &leftDockID, &mainDockID);
		ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Right, 0.3f, &rightDockID, &mainDockID);
		ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Down, 0.3f, &downDockID, &mainDockID);

		// Dock windows by name
		ImGui::DockBuilderDockWindow("Inspector", rightDockID);
		ImGui::DockBuilderDockWindow("Hierarchy", leftDockID);
		ImGui::DockBuilderDockWindow("Viewport", mainDockID);
		ImGui::DockBuilderDockWindow("Settings", downDockID);

		ImGui::DockBuilderFinish(dockID);
	}

	for (auto& pGUIWin : g.m_GUIWindows)
		pGUIWin->Update(dt);
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
