#ifndef GLFWWINDOW_H
#define GLFWWINDOW_H
#include "Window/BaseWindow.h"

struct GLFWwindow; struct GLFWcursor; struct HWND__; typedef HWND__* HWND;

namespace rdx
{
	class GLFWWindow : public BaseWindow
	{
		inline ~GLFWWindow() {};

		bool Init() override;
		bool Terminate() override;
		void PollEvents() override;

		bool IsWindowShouldClose() override;

	private:
		KeyCode TranslateKey(const int key);
		void RegisterCallbacks();

	private:
		GLFWwindow* m_pWindow{ nullptr };
		HWND m_pWindowHandle{};
	};
}

#endif