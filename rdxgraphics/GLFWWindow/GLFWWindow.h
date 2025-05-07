#pragma once
struct GLFWwindow; struct GLFWcursor; struct HWND__; typedef HWND__* HWND;

class GLFWWindow : public BaseSingleton<GLFWWindow>
{
	RX_SINGLETON_DECLARATION(GLFWWindow);
public:
	static bool Init();
	static void Terminate();
	static void StartFrame();
	static void EndFrame();

	static void ToggleMinMaxWindow();
	static void SetWindowTitle(std::string const& title);
	static void CenterCursor();
	static void SetInvisibleCursor(bool b);

	static bool IsWindowShouldClose();
	static void SetWindowShouldClose();
	static bool IsFocused();
	static void MakeContextCurrent();

	inline static GLFWwindow* GetWindowPointer() { return g.m_pWindow; }

	static glm::ivec2 GetCursorPos();
	inline static int GetCursorPosX() { return GetCursorPos().x; }
	inline static int GetCursorPosY() { return GetCursorPos().y; }

	static glm::ivec2 GetWindowPos();
	inline static int GetWindowPosX() { return GetWindowPos().x; }
	inline static int GetWindowPosY() { return GetWindowPos().y; }

	static glm::ivec2 GetWindowDims();
	inline static int GetWindowDimsX() { return GetWindowDims().x; }
	inline static int GetWindowDimsY() { return GetWindowDims().y; }

	static void SetIsVSync(bool b);
	inline static bool& IsVSync() { return g.m_IsVSync; }

private:
	static void RegisterCallbacks();

private:
	GLFWwindow* m_pWindow{ nullptr };
	HWND m_pWindowHandle{};

	bool m_IsVSync{ false };
};