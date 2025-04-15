#include <pch.h>
#include "Utils/Input.h"
#include "GLFWWindow/GLFWWindow.h"

int main()
{
	BS::synced_stream sync_out;
	BS::thread_pool tp{};
	
	tp.submit_task(
		[&sync_out]
		{
			sync_out.println("Thread pool library!");
		})
		.wait();

	int succ = glfwInit();
	GLFWWindow::Init();
	GLFWwindow* pWindow = GLFWWindow::GetWindowPointer();

	while (!GLFWWindow::IsWindowShouldClose())
	{
		GLFWWindow::StartFrame();

		if (Input::IsKeyTriggered(GLFW_KEY_ESCAPE))
			GLFWWindow::SetWindowShouldClose();

		if (Input::IsKeyTriggered(GLFW_KEY_F11))
			GLFWWindow::ToggleMinMaxWindow();
	}

	GLFWWindow::Terminate();
	return 0;
}
