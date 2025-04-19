#pragma once

class Input : public BaseSingleton<Input>
{
	RX_SINGLETON_DECLARATION(Input);
public:
	static void Init();
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

private:
	std::array<bool, GLFW_KEY_LAST + 1>			 m_Keys{ false };
	std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_Buttons{ false };
	std::array<bool, GLFW_KEY_LAST + 1>			 m_KeysPrev{ false };
	std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_ButtonsPrev{ false };
};