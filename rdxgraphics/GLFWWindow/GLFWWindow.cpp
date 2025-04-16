#include <pch.h>
#include "GLFWWindow.h"

#include "Utils/Input.h"

RX_SINGLETON_EXPLICIT(GLFWWindow)

bool GLFWWindow::Init()
{
	glfwSetErrorCallback(
		[](int errCode, const char* desc)
		{
			//FF_ERROR(R"(GLFW Error: {}, "{}")", errCode, desc);
			fprintf(stderr, "GLFW Error: %d, %s", errCode, desc);
		}
	);

	auto result = glfwInit(); RX_UNREF_PARAM(result);
	// FF_ASSERT(result == GLFW_TRUE, "glfwInit - Failed to initialize GLFW");
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	GLFWvidmode const* videoMode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);						// oGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);		// Modern oGL
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);						// Double buffering
	glfwWindowHint(GLFW_RED_BITS,	  videoMode->redBits);				// Window color depth
	glfwWindowHint(GLFW_GREEN_BITS,	  videoMode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS,	  videoMode->blueBits);
	glfwWindowHint(GLFW_ALPHA_BITS,	  videoMode->redBits);				// Using same size as red
	glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);			// Refresh rate
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);

	{ // Default size setting 
		float two_thirds = 2.f / 3.f;
		int width  = videoMode->width  * two_thirds;
		int height = videoMode->height * two_thirds;

		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		g.m_pWindow = glfwCreateWindow(width, height, "Hello", nullptr, nullptr);

		int x{}, y{};
		glfwGetMonitorPos(monitor, &x, &y);
		x += static_cast<int>(static_cast<float>(videoMode->width  - width)  * 0.5f);
		y += static_cast<int>(static_cast<float>(videoMode->height - height) * 0.5f);

		glfwSetWindowPos(g.m_pWindow, x, y);
		glfwShowWindow(g.m_pWindow);
	}
	// assert pwindow
	g.m_pWindowHandle = glfwGetWin32Window(g.m_pWindow);
	glfwSetInputMode(g.m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glfwMakeContextCurrent(g.m_pWindow);
	glfwSwapInterval(1); // Enable vsync

	Input::Init();
	RegisterCallbacks();

	return true;
}

void GLFWWindow::Terminate()
{
	glfwDestroyWindow(g.m_pWindow);
	glfwTerminate();
}

void GLFWWindow::StartFrame()
{
	Input::SwapKeys();
	glfwPollEvents();
}

void GLFWWindow::EndFrame()
{
	glfwSwapBuffers(g.m_pWindow);
}

void GLFWWindow::ToggleMinMaxWindow()
{
	int maximized = glfwGetWindowAttrib(g.m_pWindow, GLFW_MAXIMIZED);
	if (maximized == GLFW_TRUE)
		glfwRestoreWindow(g.m_pWindow);
	else
		glfwMaximizeWindow(g.m_pWindow);
}

void GLFWWindow::RegisterCallbacks()
{
	glfwSetWindowPosCallback(g.m_pWindow,
		[](GLFWwindow*, int xpos, int ypos)
		{
			Input::WindowPosCallback(xpos, ypos);
		});

	glfwSetWindowSizeCallback(g.m_pWindow,
		[](GLFWwindow*, int width, int height)
		{
			EventDispatcher<int, int>::FireEvent(RX_EVENT_WINDOW_RESIZE, width, height);
		});

	glfwSetFramebufferSizeCallback(g.m_pWindow,
		[](GLFWwindow*, int width, int height)
		{
			EventDispatcher<int, int>::FireEvent(RX_EVENT_FRAMEBUFFER_RESIZE, width, height);
		});

	glfwSetKeyCallback(g.m_pWindow,
		[](GLFWwindow*, int key, int scancode, int action, int)
		{
			if (!GLFWWindow::IsFocused())
				return;

			Input::KeyCallback(key, scancode, action);
		});

	glfwSetMouseButtonCallback(g.m_pWindow,
		[](GLFWwindow*, int button, int action, int)
		{
			if (!GLFWWindow::IsFocused())
				return;

			Input::ButtonCallback(button, action);
		});

	glfwSetCursorPosCallback(g.m_pWindow,
		[](GLFWwindow*, double xpos, double ypos)
		{
			if (!GLFWWindow::IsFocused())
				return;

			EventDispatcher<double, double>::FireEvent(
				RX_EVENT_CURSOR_POS_CALLBACK,
				xpos, ypos);
		});

	glfwSetScrollCallback(g.m_pWindow,
		[](GLFWwindow*, double xoffset, double yoffset)
		{
			if (!GLFWWindow::IsFocused())
				return;

			EventDispatcher<double, double>::FireEvent(
				RX_EVENT_SCROLL_CALLBACK,
				xoffset, yoffset);
		});
}
