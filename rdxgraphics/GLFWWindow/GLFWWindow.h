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
	static void CenterCursor();
	inline static void SetInvisibleCursor(bool b) { glfwSetInputMode(g.m_pWindow, GLFW_CURSOR, b ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL); }

	inline static bool IsWindowShouldClose() { return glfwWindowShouldClose(g.m_pWindow); }
	inline static void SetWindowShouldClose() { glfwSetWindowShouldClose(g.m_pWindow, true); }
	inline static bool IsFocused() { return glfwGetWindowAttrib(g.m_pWindow, GLFW_FOCUSED); }

	inline static GLFWwindow* GetWindowPointer() { return g.m_pWindow; }

	inline static glm::ivec2 GetCursorPos() { double x{}, y{}; glfwGetCursorPos(g.m_pWindow, &x, &y); return { (int)x, (int)y }; }
	inline static int GetCursorPosX() { return GetCursorPos().x; }
	inline static int GetCursorPosY() { return GetCursorPos().y; }

	inline static glm::ivec2 GetWindowPos() { int x{}, y{}; glfwGetWindowPos(g.m_pWindow, &x, &y); return { (int)x, (int)y }; }
	inline static int GetWindowPosX() { return GetWindowPos().x; }
	inline static int GetWindowPosY() { return GetWindowPos().y; }

	inline static glm::ivec2 GetWindowDims() { int x{}, y{}; glfwGetWindowSize(g.m_pWindow, &x, &y); return { (int)x, (int)y }; }
	inline static int GetWindowDimsX() { return GetWindowDims().x; }
	inline static int GetWindowDimsY() { return GetWindowDims().y; }

private:
	static void RegisterCallbacks();

private:
	GLFWwindow* m_pWindow{ nullptr };
	HWND m_pWindowHandle{};
};