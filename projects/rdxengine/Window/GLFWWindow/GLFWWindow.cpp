#include "GLFWWindow.h"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

using namespace rdx;

bool GLFWWindow::Init()
{
	glfwSetErrorCallback(
		[](int code, const char* desc)
		{
			std::cerr << "GLFW Error code " << code << " - " << desc << std::endl;
		});

	if (glfwInit() == GLFW_FALSE)
	{
		std::cerr << "Failed to initialize GLFW." << std::endl;
		return false;
	}

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	GLFWvidmode const* videoMode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);						// oGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);		// Modern oGL
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);						// Double buffering
	glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);				// Window color depth
	glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
	glfwWindowHint(GLFW_ALPHA_BITS, videoMode->redBits);				// Using same size as red
	glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);			// Refresh rate
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);

	{ // Default size setting 
		float two_thirds = 2.f / 3.f;
		int width = static_cast<int>(static_cast<float>(videoMode->width) * two_thirds);
		int height = static_cast<int>(static_cast<float>(videoMode->height) * two_thirds);

		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		m_pWindow = glfwCreateWindow(width, height, "Hello", nullptr, nullptr);

		int x{}, y{};
		glfwGetMonitorPos(monitor, &x, &y);
		x += static_cast<int>(static_cast<float>(videoMode->width - width) * 0.5f);
		y += static_cast<int>(static_cast<float>(videoMode->height - height) * 0.5f);

		glfwSetWindowPos(m_pWindow, x, y);
		glfwShowWindow(m_pWindow);
	}

	m_pWindowHandle = glfwGetWin32Window(m_pWindow);
	glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glfwMakeContextCurrent(m_pWindow);
	//SetIsVSync(false); // Disable vsync

	//Input::Init();
	RegisterCallbacks();

	//SetTargetFPS(30); // Hardcoded to 30FPS by default.

	return true;
}

bool GLFWWindow::Terminate()
{
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
	return true;
}

void GLFWWindow::PollEvents()
{
	glfwPollEvents();
}

bool GLFWWindow::IsWindowShouldClose()
{
	return glfwWindowShouldClose(m_pWindow);
}

KeyCode GLFWWindow::TranslateKey(const int key)
{
	switch (key)
	{
	case GLFW_KEY_SPACE: return KeyCode::Space;
	case GLFW_KEY_APOSTROPHE: return KeyCode::Apostrophe;
	case GLFW_KEY_COMMA: return KeyCode::Comma;
	case GLFW_KEY_MINUS: return KeyCode::Minus;
	case GLFW_KEY_PERIOD: return KeyCode::Period;
	case GLFW_KEY_SLASH: return KeyCode::Slash;
	case GLFW_KEY_0: return KeyCode::Alpha0;
	case GLFW_KEY_1: return KeyCode::Alpha1;
	case GLFW_KEY_2: return KeyCode::Alpha2;
	case GLFW_KEY_3: return KeyCode::Alpha3;
	case GLFW_KEY_4: return KeyCode::Alpha4;
	case GLFW_KEY_5: return KeyCode::Alpha5;
	case GLFW_KEY_6: return KeyCode::Alpha6;
	case GLFW_KEY_7: return KeyCode::Alpha7;
	case GLFW_KEY_8: return KeyCode::Alpha8;
	case GLFW_KEY_9: return KeyCode::Alpha9;
	case GLFW_KEY_SEMICOLON: return KeyCode::Semicolon;
	case GLFW_KEY_EQUAL: return KeyCode::Equal;
	case GLFW_KEY_A: return KeyCode::A;
	case GLFW_KEY_B: return KeyCode::B;
	case GLFW_KEY_C: return KeyCode::C;
	case GLFW_KEY_D: return KeyCode::D;
	case GLFW_KEY_E: return KeyCode::E;
	case GLFW_KEY_F: return KeyCode::F;
	case GLFW_KEY_G: return KeyCode::G;
	case GLFW_KEY_H: return KeyCode::H;
	case GLFW_KEY_I: return KeyCode::I;
	case GLFW_KEY_J: return KeyCode::J;
	case GLFW_KEY_K: return KeyCode::K;
	case GLFW_KEY_L: return KeyCode::L;
	case GLFW_KEY_M: return KeyCode::M;
	case GLFW_KEY_N: return KeyCode::N;
	case GLFW_KEY_O: return KeyCode::O;
	case GLFW_KEY_P: return KeyCode::P;
	case GLFW_KEY_Q: return KeyCode::Q;
	case GLFW_KEY_R: return KeyCode::R;
	case GLFW_KEY_S: return KeyCode::S;
	case GLFW_KEY_T: return KeyCode::T;
	case GLFW_KEY_U: return KeyCode::U;
	case GLFW_KEY_V: return KeyCode::V;
	case GLFW_KEY_W: return KeyCode::W;
	case GLFW_KEY_X: return KeyCode::X;
	case GLFW_KEY_Y: return KeyCode::Y;
	case GLFW_KEY_Z: return KeyCode::Z;
	case GLFW_KEY_BACKSLASH: return KeyCode::BackSlash;
	case GLFW_KEY_LEFT_BRACKET: return KeyCode::LBracket;
	case GLFW_KEY_RIGHT_BRACKET: return KeyCode::RBracket;
	case GLFW_KEY_GRAVE_ACCENT: return KeyCode::GraveAccent;
	case GLFW_KEY_ESCAPE: return KeyCode::Escape;
	case GLFW_KEY_ENTER: return KeyCode::Enter;
	case GLFW_KEY_TAB: return KeyCode::Tab;
	case GLFW_KEY_BACKSPACE: return KeyCode::Backspace;
	case GLFW_KEY_INSERT: return KeyCode::Insert;
	case GLFW_KEY_DELETE: return KeyCode::Delete;
	case GLFW_KEY_RIGHT: return KeyCode::Right;
	case GLFW_KEY_LEFT: return KeyCode::Left;
	case GLFW_KEY_DOWN: return KeyCode::Down;
	case GLFW_KEY_UP: return KeyCode::Up;
	case GLFW_KEY_PAGE_UP: return KeyCode::PageUp;
	case GLFW_KEY_PAGE_DOWN: return KeyCode::PageDown;
	case GLFW_KEY_HOME: return KeyCode::Home;
	case GLFW_KEY_END: return KeyCode::End;
	case GLFW_KEY_CAPS_LOCK: return KeyCode::CapsLock;
	case GLFW_KEY_SCROLL_LOCK: return KeyCode::ScrollLock;
	case GLFW_KEY_NUM_LOCK: return KeyCode::NumLock;
	case GLFW_KEY_PRINT_SCREEN: return KeyCode::PrintScreen;
	case GLFW_KEY_PAUSE: return KeyCode::Pause;
	case GLFW_KEY_F1: return KeyCode::F1;
	case GLFW_KEY_F2: return KeyCode::F2;
	case GLFW_KEY_F3: return KeyCode::F3;
	case GLFW_KEY_F4: return KeyCode::F4;
	case GLFW_KEY_F5: return KeyCode::F5;
	case GLFW_KEY_F6: return KeyCode::F6;
	case GLFW_KEY_F7: return KeyCode::F7;
	case GLFW_KEY_F8: return KeyCode::F8;
	case GLFW_KEY_F9: return KeyCode::F9;
	case GLFW_KEY_F10: return KeyCode::F10;
	case GLFW_KEY_F11: return KeyCode::F11;
	case GLFW_KEY_F12: return KeyCode::F12;
	case GLFW_KEY_KP_0: return KeyCode::KeyPad_0;
	case GLFW_KEY_KP_1: return KeyCode::KeyPad_1;
	case GLFW_KEY_KP_2: return KeyCode::KeyPad_2;
	case GLFW_KEY_KP_3: return KeyCode::KeyPad_3;
	case GLFW_KEY_KP_4: return KeyCode::KeyPad_4;
	case GLFW_KEY_KP_5: return KeyCode::KeyPad_5;
	case GLFW_KEY_KP_6: return KeyCode::KeyPad_6;
	case GLFW_KEY_KP_7: return KeyCode::KeyPad_7;
	case GLFW_KEY_KP_8: return KeyCode::KeyPad_8;
	case GLFW_KEY_KP_9: return KeyCode::KeyPad_9;
	case GLFW_KEY_KP_DECIMAL: return KeyCode::KeyPad_Decimal;
	case GLFW_KEY_KP_DIVIDE: return KeyCode::KeyPad_Divide;
	case GLFW_KEY_KP_MULTIPLY: return KeyCode::KeyPad_Multiply;
	case GLFW_KEY_KP_SUBTRACT: return KeyCode::KeyPad_Subtract;
	case GLFW_KEY_KP_ADD: return KeyCode::KeyPad_Add;
	case GLFW_KEY_KP_ENTER: return KeyCode::KeyPad_Enter;
	case GLFW_KEY_KP_EQUAL: return KeyCode::KeyPad_Equal;
	case GLFW_KEY_LEFT_SHIFT: return KeyCode::LShift;
	case GLFW_KEY_LEFT_CONTROL: return KeyCode::LCtrl;
	case GLFW_KEY_LEFT_ALT: return KeyCode::LAlt;
	case GLFW_KEY_RIGHT_SHIFT: return KeyCode::RShift;
	case GLFW_KEY_RIGHT_CONTROL: return KeyCode::RCtrl;
	case GLFW_KEY_RIGHT_ALT: return KeyCode::RAlt;
	default: break;
	}
	return KeyCode::Unknown;
}

void GLFWWindow::RegisterCallbacks()
{
	glfwSetKeyCallback(m_pWindow,
		[](GLFWwindow* window, int key, int, int action, int)
		{
			//if (!GLFWWindow::IsFocused())
			//	return;
			GLFWWindow* pWin = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
			
			//std::cout << action << std::endl;
			switch (action)
			{
				case GLFW_RELEASE:
					pWin->OnKeyRelease(pWin->TranslateKey(key));
					break;
				case GLFW_PRESS:
				case GLFW_REPEAT:
					pWin->OnKeyPress(pWin->TranslateKey(key));
					break;
				default: break;
			}
		});
}
