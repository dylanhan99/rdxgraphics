#include "RDXGui.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include "rdxengine/ServiceLayer.h"
#include "rdxengine/Event/Events/Events.h"
#include "rdxengine/Window/GLFWWindow.h" // Should not be like this, but it's easier to just do this rn
#include "Panels/EngineProfiler.h"
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

	RegisterPanel<EngineProfiler>("Profiler", 0);

	return true;
}

bool RDXGui::Terminate()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	return true;
}

void RDXGui::FrameStartImpl()
{

}

void RDXGui::FrameEndImpl()
{
	{
		std::string title{ "RDXEditor [" };
		title += std::to_string(rdx::ServiceLayer::FrameRateControllerService()->GetEstimatedFPS()) + "]";

		rdx::ServiceLayer::InstantEventService()->Publish(rdx::ChangeWindowTitleEvent{ std::move(title) });
	}

	{
		auto* pInput = rdx::ServiceLayer::InputService();
		RX_ASSERT(pInput);
		if (pInput->IsKeyDown(rdx::KeyCode::LCtrl) && pInput->IsKeyTriggered(rdx::KeyCode::E))
			m_IsEnabled = !m_IsEnabled;
	}

	if (!IsEnabled())
		return;

	constexpr uint32_t dockID = 67;

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

			uint32_t mainDockID = dockID;
			ImGui::DockBuilderDockWindow("Profiler", mainDockID);

			ImGui::DockBuilderFinish(dockID);
			dockLoaded = true;
		}
	}

	{ // Update
		MenuBar();
		for (auto& pPanel : m_Panels)
			pPanel->Update(0.f);
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
