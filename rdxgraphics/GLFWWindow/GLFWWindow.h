#pragma once

class GLFWWindow : public BaseSingleton<GLFWWindow>
{
	RX_SINGLETON_DECLARATION(GLFWWindow);
public:
	static bool Init();
	static void Terminate();
	static void StartFrame();
	static void EndFrame();

	static void ToggleMinMaxWindow();

	inline static bool IsWindowShouldClose() { return glfwWindowShouldClose(g.m_pWindow); }
	inline static void SetWindowShouldClose() { glfwSetWindowShouldClose(g.m_pWindow, true); }
	inline static bool IsFocused() { return glfwGetWindowAttrib(g.m_pWindow, GLFW_FOCUSED); }

	inline static GLFWwindow* GetWindowPointer() { return g.m_pWindow; }

private:
	static void RegisterCallbacks();

private:
	GLFWwindow* m_pWindow{ nullptr };
	HWND m_pWindowHandle{};
};