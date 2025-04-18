#include <pch.h>
#include "RDX.h"

#include "Utils/Input.h"
#include "GLFWWindow/GLFWWindow.h"
#include "Rendering/RenderSystem.h"

void RDX::Run()
{
	Logger::Init();

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

	while (!GLFWWindow::IsWindowShouldClose())
	{
		GLFWWindow::StartFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

		if (Input::IsKeyTriggered(GLFW_KEY_ESCAPE))
			GLFWWindow::SetWindowShouldClose();

		if (Input::IsKeyTriggered(GLFW_KEY_F11))
			GLFWWindow::ToggleMinMaxWindow();

		if (Input::IsKeyTriggered(GLFW_KEY_F5))
			RenderSystem::ReloadShaders();

		{ // ImGui update
			ImGui::Begin("Hi", nullptr, 0);
			{
				ImGui::ColorEdit3("Back Buffer Color", glm::value_ptr(RenderSystem::GetBackBufferColor()));
			}
			ImGui::End();
			ImGui::Render();
		}

		// Render
		{
			RenderSystem::Update(0.0);

			glDisable(GL_DEPTH_TEST);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		GLFWWindow::EndFrame();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	RenderSystem::Terminate();
	GLFWWindow::Terminate();

	Logger::Terminate();
}
