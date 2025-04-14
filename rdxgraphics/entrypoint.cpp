#include <pch.h>
#include <GLFW/glfw3.h>

int main()
{
	int succ = glfwInit();
	GLFWwindow* pWindow = glfwCreateWindow(600, 400, "Hello", nullptr, nullptr);

	while (!glfwWindowShouldClose(pWindow))
	{
		glfwSwapBuffers(pWindow);
		glfwPollEvents();

		if (glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(pWindow, true);
	}

	glfwTerminate();
	return 0;
}
