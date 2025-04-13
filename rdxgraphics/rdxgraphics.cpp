#include <iostream>
#include "rdxgraphics.h"
#include <GLFW/glfw3.h>

int main()
{
	int succ = glfwInit();

	std::cout << succ << std::endl;

	glfwTerminate();
	return 0;
}
