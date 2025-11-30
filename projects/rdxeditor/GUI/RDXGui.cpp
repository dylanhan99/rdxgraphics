#include "RDXGui.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include "rdxengine/ServiceLayer.h"
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
			ImGui::DockBuilderDockWindow("test", mainDockID);
			ImGui::DockBuilderDockWindow("Engine Profiler", mainDockID);

			ImGui::DockBuilderFinish(dockID);
			dockLoaded = true;
		}
	}

	{ // Update
		if (ImGui::Begin("test", nullptr))
			;
		ImGui::End();

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