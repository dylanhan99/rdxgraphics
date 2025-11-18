#ifndef GLFWWINDOW_H
#define GLFWWINDOW_H
#include "Window/BaseWindow.h"

struct GLFWwindow; struct GLFWcursor; struct HWND__; typedef HWND__* HWND;

namespace rdx
{
	class GLFWWindow final : public BaseWindow
	{
	public:
		inline ~GLFWWindow() {};

		bool InitImpl() override;
		bool TerminateImpl() override;
		void SwapBuffers() override;

		bool IsWindowShouldClose() override;
		void SetShouldClose() override;

	private:
		void PollEventsImpl() override;
		KeyCode TranslateKey(const int key);
		void RegisterCallbacks();

	private:
		GLFWwindow* m_pWindow{ nullptr };
		HWND m_pWindowHandle{};
	};
}

#endif