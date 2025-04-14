#pragma once

class Input : public BaseSingleton<Input>
{
	RX_SINGLETON_DECLARATION(Input);
public:
	static void SwapKeys();

	static void KeyCallback(int key, int scancode, int action);
	static void ButtonCallback(int btn, int action);

	static bool IsKeyTriggered(unsigned int key);
	static bool IsKeyReleased(unsigned int key);
	static bool IsKeyDown(unsigned int key);
	static bool IsKeyUp(unsigned int key);

	static bool IsMouseTriggered(unsigned int key);
	static bool IsMouseReleased(unsigned int key);
	static bool IsMouseDown(unsigned int key);
	static bool IsMouseUp(unsigned int key);

	inline static glm::ivec2 GetCursor() { return m_Cursor; };
	inline static int GetCursorX() { return m_Cursor.x; }
	inline static int GetCursorY() { return m_Cursor.y; }

private:
	inline static std::array<bool, GLFW_KEY_LAST + 1>		   m_Keys{ false };
	inline static std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_Buttons{ false };
	inline static std::array<bool, GLFW_KEY_LAST + 1>		   m_KeysPrev{ false };
	inline static std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_ButtonsPrev{ false };
	inline static glm::ivec2 m_Cursor{};
};