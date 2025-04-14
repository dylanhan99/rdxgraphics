#pragma once

struct GLFWwindow; struct GLFWcursor; struct HWND__; typedef HWND__* HWND;

class GLFWWindow : public BaseSingleton<GLFWWindow>
{
	RX_SINGLETON_DECLARATION(GLFWWindow);
public:
	static void Init();
	static void Terminate();

	inline static GLFWwindow* GetWindowPointer() { return g.m_pWindow; }

private:
	GLFWwindow* m_pWindow{ nullptr };
	HWND m_pWindowHandle{};
};