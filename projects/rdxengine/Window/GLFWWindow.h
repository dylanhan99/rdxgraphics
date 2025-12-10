#ifndef GLFWWINDOW_H
#define GLFWWINDOW_H
#include "rdxengine/Window/BaseWindow.h"

struct GLFWwindow; struct GLFWcursor; struct HWND__; typedef HWND__* HWND;

namespace rdx
{
	class GLFWWindow final : public BaseWindow
	{
	public:
		~GLFWWindow() override;

		bool InitImpl() override;
		bool TerminateImpl() override;
		void SwapBuffers() override;

		bool IsWindowShouldClose() override;
		void SetShouldClose() override;

		glm::vec2 GetWindowDims() const override;
		void SetMousePos(int xpos, int ypos) const override;

		GLFWwindow* GetWindowPointer() { return m_pWindow; }
		void SetContextCurrent() const;

	private:
		void PollEventsImpl() override;
		KeyCode TranslateKey(const int key);
		MouseCode TranslateMouse(const int button);
		void RegisterCallbacks();

	private:
		GLFWwindow* m_pWindow{ nullptr };
		HWND m_pWindowHandle{};
	};
}

#endif