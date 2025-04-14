#pragma once

class Input : public BaseSingleton<Input>
{
	RX_SINGLETON_DECLARATION(Input);
public:
	static void Init();
	static void SwapKeys();

	static void KeyCallback(int key, int scancode, int action);
	static void ButtonCallback(int btn, int action);
	static void CursorPosCallback(int xpos, int ypos);
	static void WindowPosCallback(int xpos, int ypos);
	static void WindowSizeCallback(int width, int height);

	static bool IsKeyTriggered(unsigned int key);
	static bool IsKeyReleased(unsigned int key);
	static bool IsKeyDown(unsigned int key);
	static bool IsKeyUp(unsigned int key);

	static bool IsMouseTriggered(unsigned int key);
	static bool IsMouseReleased(unsigned int key);
	static bool IsMouseDown(unsigned int key);
	static bool IsMouseUp(unsigned int key);

	inline static glm::ivec2 GetCursorPos() { return g.m_CursorPos; };
	inline static int GetCursorPosX() { return g.m_CursorPos.x; }
	inline static int GetCursorPosY() { return g.m_CursorPos.y; }

	inline static glm::ivec2 GetWindowPos() { return g.m_WindowPos; }
	inline static int GetWindowPosX() { return g.m_WindowPos.x; }
	inline static int GetWindowPosY() { return g.m_WindowPos.y; }

	inline static glm::ivec2 GetWindowDims() { return g.m_WindowDims; }
	inline static int GetWindowDimsX() { return g.m_WindowDims.x; }
	inline static int GetWindowDimsY() { return g.m_WindowDims.y; }

private:
	std::array<bool, GLFW_KEY_LAST + 1>			 m_Keys{ false };
	std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_Buttons{ false };
	std::array<bool, GLFW_KEY_LAST + 1>			 m_KeysPrev{ false };
	std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_ButtonsPrev{ false };
	glm::ivec2 m_CursorPos{};
	glm::ivec2 m_WindowPos{};
	glm::ivec2 m_WindowDims{};
};