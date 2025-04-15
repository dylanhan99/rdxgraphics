#include <pch.h>
#include "RDX.h"

#include "Utils/Input.h"
#include "GLFWWindow/GLFWWindow.h"

void RDX::Run()
{
	BS::synced_stream sync_out;
	BS::thread_pool tp{};

	tp.submit_task(
		[&sync_out]
		{
			sync_out.println("Thread pool library!");
		})
		.wait();

	GLFWWindow::Init();
	GLFWwindow* pWindow = GLFWWindow::GetWindowPointer();

	if (!gladLoadGL(glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return;
	}

	std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;

	while (!GLFWWindow::IsWindowShouldClose())
	{
		GLFWWindow::StartFrame();

		if (Input::IsKeyTriggered(GLFW_KEY_ESCAPE))
			GLFWWindow::SetWindowShouldClose();

		if (Input::IsKeyTriggered(GLFW_KEY_F11))
			GLFWWindow::ToggleMinMaxWindow();
	}

	GLFWWindow::Terminate();
}
