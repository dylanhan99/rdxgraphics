#include <pch.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "GLFWWindow.h"
#include "Utils/Input.h"

RX_SINGLETON_EXPLICIT(GLFWWindow)

bool GLFWWindow::Init()
{
	glfwSetErrorCallback(
		[](int errCode, const char* desc)
		{
			RX_ERROR(R"(GLFW Error: {}, "{}")", errCode, desc);
		}
	);

	auto result = glfwInit(); RX_UNREF_PARAM(result);
	RX_ASSERT(result == GLFW_TRUE, "Failed to initialize GLFW");
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	GLFWvidmode const* videoMode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);						// oGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
		int width  = static_cast<int>(static_cast<float>(videoMode->width)  * two_thirds);
		int height = static_cast<int>(static_cast<float>(videoMode->height) * two_thirds);

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
	SetIsVSync(false); // Disable vsync

	Input::Init();
	RegisterCallbacks();

	SetTargetFPS(30); // Hardcoded to 30FPS by default.

	return true;
}

void GLFWWindow::Terminate()
{
	glfwDestroyWindow(g.m_pWindow);
	glfwTerminate();
}

void GLFWWindow::ToggleMinMaxWindow()
{
	int maximized = glfwGetWindowAttrib(g.m_pWindow, GLFW_MAXIMIZED);
	if (maximized == GLFW_TRUE)
		glfwRestoreWindow(g.m_pWindow);
	else
		glfwMaximizeWindow(g.m_pWindow);
}

void GLFWWindow::SetWindowTitle(std::string const& title) 
{
	glfwSetWindowTitle(g.m_pWindow, title.c_str()); 
}

void GLFWWindow::CenterCursor()
{
	glm::vec2 winDims = (glm::vec2)GetWindowDims();
	glfwSetCursorPos(g.m_pWindow, (int)(winDims.x * 0.5f), (int)(winDims.y * 0.5f));
}

void GLFWWindow::SetInvisibleCursor(bool b) 
{ 
	glfwSetInputMode(g.m_pWindow, GLFW_CURSOR, b ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL); 
}

bool GLFWWindow::IsWindowShouldClose() 
{ 
	return glfwWindowShouldClose(g.m_pWindow); 
}

void GLFWWindow::SetWindowShouldClose() 
{ 
	glfwSetWindowShouldClose(g.m_pWindow, true); 
}

bool GLFWWindow::IsFocused() 
{ 
	return glfwGetWindowAttrib(g.m_pWindow, GLFW_FOCUSED); 
}

void GLFWWindow::MakeContextCurrent()
{
	glfwMakeContextCurrent(g.m_pWindow);
}

glm::ivec2 GLFWWindow::GetCursorPos() 
{ 
	double x{}, y{}; 
	glfwGetCursorPos(g.m_pWindow, &x, &y); 
	return { (int)x, (int)y }; 
}

glm::ivec2 GLFWWindow::GetWindowPos()
{
	int x{}, y{};
	glfwGetWindowPos(g.m_pWindow, &x, &y);
	return { (int)x, (int)y };
}

glm::ivec2 GLFWWindow::GetWindowDims() 
{ 
	int x{}, y{}; 
	glfwGetWindowSize(g.m_pWindow, &x, &y); 
	return { (int)x, (int)y }; 
}

void GLFWWindow::SetIsVSync(bool b) 
{ 
	g.m_IsVSync = b; 
	glfwSwapInterval((int)b); 
}

void GLFWWindow::RegisterCallbacks()
{
	glfwSetWindowPosCallback(g.m_pWindow,
		[](GLFWwindow*, int xpos, int ypos)
		{
			//Input::WindowPosCallback(xpos, ypos);
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
			
			Input::ScrollCallback(xoffset, yoffset);
		});
}

void GLFWWindow::Update(std::function<void(double)> fnUpdate)
{
	{ // FRC start gameloop
		g.m_PrevTime = g.m_CurrTime;
		g.m_CurrTime = glfwGetTime();
		double dt = g.m_CurrTime - g.m_PrevTime;

		g.m_AccumulatedDT += dt;
	}

	if (g.m_AccumulatedDT <= g.m_TargetDT)
		return;

	{ // FRC start frame
		g.m_FPS = static_cast<uint32_t>(1.0 / g.m_AccumulatedDT);
		g.m_IntervalTimer += GetDT();
		if (g.m_IntervalTimer >= 1.0)
		{
			static float alpha = 0.6f; // smoothing factor
			g.m_IntervalFPS = (uint32_t)((1.f - alpha) * (float)g.m_IntervalFPS + alpha * (float)g.m_FPS);

			g.m_IntervalTimer = 0.0;
			//g.m_IntervalFPS = g.m_FPS;
		}
	}

	{ // GLFW start frame
		Input::SwapKeys();
		glfwPollEvents();
	}

	if (fnUpdate) 
		fnUpdate(GetDT());

	// End frame
	g.m_AccumulatedDT = 0.0;
	glfwSwapBuffers(g.m_pWindow);
}

void GLFWWindow::SetTargetFPS(uint32_t target)
{
	g.m_TargetFPS = target;
	g.m_TargetDT = 1.0 / g.m_TargetFPS;
}
