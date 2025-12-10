#include "RDXGui.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include "rdxengine/ServiceLayer.h"
#include "rdxengine/Event/Events/Events.h"
#include "rdxengine/Window/GLFWWindow.h" // Should not be like this, but it's easier to just do this rn
#include "Panels/EngineProfiler.h"
#include "Panels/Viewport.h"
#include "Panels/Hierarchy.h"
#include "Panels/Inspector.h"

using namespace rdxgui;

// In the future, for stuff like OpenGL3 and GLFW, need to have precompile flags to ensure the correct
// Function or header is being used.

bool RDXGui::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::SetCurrentContext(ImGui::GetCurrentContext());

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		  // Enable detatchable imgui windows

	io.IniFilename = nullptr; // Disable .ini persistence
	ImGui::StyleColorsDark();

	GLFWwindow* windowPointer = static_cast<rdx::GLFWWindow*>(rdx::ServiceLayer::WindowSystem())->GetWindowPointer();
	ImGui_ImplGlfw_InitForOpenGL(windowPointer, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	RegisterPanel<ToolBar>("Tool Bar");
	RegisterPanel<EngineProfiler>("Profiler");
	RegisterPanel<EngineViewport>("Engine");
	RegisterPanel<GameViewport>("Game");
	RegisterPanel<Hierarchy>("Hierarchy");
	RegisterPanel<Inspector>("Inspector");

	for (auto& panel : m_Panels)
		panel->Init();

	rdx::ServiceLayer::InstantEventService()->Publish(rdx::CameraChangeEvent{ &m_GameCamera });

	return true;
}

bool RDXGui::Terminate()
{
	for (auto& panel : m_Panels)
		panel->Terminate();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	m_Panels.clear();
	return true;
}

void RDXGui::FrameStartImpl()
{
	m_EditorCamera.UpdateCameraVectors(m_EditorCameraPos.Position, m_EditorCameraPos.Rotation);
}

void RDXGui::FrameEndImpl()
{
	{
		std::ostringstream ss{};
		ss << "RDXEditor ["
			<< rdx::ServiceLayer::FrameRateControllerService()->GetEstimatedFPS()
			<< "]";
		
		rdx::ServiceLayer::InstantEventService()->Publish(rdx::ChangeWindowTitleEvent{ std::move(ss.str()) });
	}

	{
		auto* pInput = rdx::ServiceLayer::InputService();
		if (pInput->IsKeyDown(rdx::KeyCode::LCtrl) && pInput->IsKeyTriggered(rdx::KeyCode::E))
			m_IsEnabled = !m_IsEnabled;
	}

	if (!IsEnabled())
		return;

	constexpr ImGuiID dockID = 67;

	{ // Frame start
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::DockSpaceOverViewport(dockID, viewport);

		static bool dockLoaded{ false };
		if (!dockLoaded) // lazy init dock
		{
			ImGui::DockBuilderRemoveNode(dockID);
			ImGui::DockBuilderAddNode(dockID, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockID, viewport->Size);

			ImGuiID mainDockID{ dockID }, leftDockID{}, rightDockID{}, downDockID{}, toolbarDockID{};
			ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Up, 0.05f, &toolbarDockID, &mainDockID);
			ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Left, 0.2f, &leftDockID, &mainDockID);
			ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Right, 0.3f, &rightDockID, &mainDockID);
			ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Down, 0.3f, &downDockID, &mainDockID);

			// Further split the main dock into left and right
			ImGuiID engineDockID{}, gameDockID{};
			ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Left, 0.5f, &engineDockID, &mainDockID);
			gameDockID = mainDockID;

			ImGui::DockBuilderDockWindow("Tool Bar", toolbarDockID);
			ImGui::DockBuilderDockWindow("Engine", engineDockID);
			ImGui::DockBuilderDockWindow("Game", gameDockID);
			ImGui::DockBuilderDockWindow("Profiler", downDockID);
			ImGui::DockBuilderDockWindow("Hierarchy", leftDockID);
			ImGui::DockBuilderDockWindow("Inspector", rightDockID);

			// Lock the toolbar node
			ImGuiDockNode* toolbarNode = ImGui::DockBuilderGetNode(toolbarDockID);
			toolbarNode->LocalFlags |=
				ImGuiDockNodeFlags_NoTabBar |
				ImGuiDockNodeFlags_NoDocking |
				ImGuiDockNodeFlags_NoSplit |
				ImGuiDockNodeFlags_NoResize;

			ImGui::DockBuilderFinish(dockID);
			dockLoaded = true;
		}
	}

	{ // Update
		MenuBar();
		for (auto& pPanel : m_Panels)
			pPanel->Update(rdx::ServiceLayer::FrameRateControllerService()->GetDT());
	}

	{ // Draw
		rdx::ServiceLayer::RenderingSystem()->SetDepthTest(false);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			static_cast<rdx::GLFWWindow*>(rdx::ServiceLayer::WindowSystem())->SetContextCurrent();
		}
	}
}

void rdxgui::RDXGui::MenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("General"))
		{
			static int s_FPSIndex = 1; // Defaults at 30
			static std::array<const char*, 7> s_FPSs{ "15", "30", "60", "120", "144", "240", "No Lim." };
			static std::array<uint32_t, 7> s_FPSsNum{ 15, 30, 60, 120, 144, 240, std::numeric_limits<uint32_t>::max() };
			bool isVSync = rdx::ServiceLayer::FrameRateControllerService()->IsVSync();
			ImGui::BeginDisabled(isVSync);
			if (ImGui::SliderInt("FPS", &s_FPSIndex, 0, static_cast<int>(s_FPSs.size() - 1), s_FPSs[s_FPSIndex]))
			{
				RX_ASSERT(0 <= s_FPSIndex && s_FPSIndex < s_FPSsNum.size());
				rdx::ServiceLayer::InstantEventService()->Publish(rdx::FPSChangedEvent{ s_FPSsNum[s_FPSIndex] });
			}
			ImGui::EndDisabled();
		
			if (ImGui::Checkbox("V-Sync", &isVSync))
			{
				rdx::ServiceLayer::InstantEventService()->Publish(rdx::ToggleVSyncEvent{ isVSync });
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
