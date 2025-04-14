#include <pch.h>
#include "Utils/Input.h"
#include "GLFWWindow/GLFWWindow.h"

int main()
{
	int succ = glfwInit();
	GLFWWindow::Init();
	GLFWwindow* pWindow = GLFWWindow::GetWindowPointer();

	while (!glfwWindowShouldClose(pWindow))
	{
		glfwSwapBuffers(pWindow);
		glfwPollEvents();

		if (Input::IsKeyTriggered(GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(pWindow, true);

		Input::SwapKeys();
	}

	GLFWWindow::Terminate();
	return 0;
}
