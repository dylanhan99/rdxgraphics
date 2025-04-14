#include <pch.h>
#include "Input.h"

RX_SINGLETON_EXPLICIT(Input)

void Input::SwapKeys()
{
	std::ranges::copy(g.m_Keys.begin(), g.m_Keys.end(), g.m_KeysPrev.begin());
	std::ranges::copy(g.m_Buttons.begin(), g.m_Buttons.end(), g.m_ButtonsPrev.begin());
}

void Input::KeyCallback(int key, int scancode, int action)
{
	RX_UNREF_PARAM(scancode);

	if (key == GLFW_KEY_UNKNOWN)
		return;

	m_KeysPrev[key] = m_Keys[key];
	m_Keys[key] = action;
}

void Input::ButtonCallback(int btn, int action)
{
	if (btn == GLFW_KEY_UNKNOWN)
		return;

	m_ButtonsPrev[btn] = m_Buttons[btn];
	m_Buttons[btn] = action;
}

bool Input::IsKeyTriggered(unsigned int key)
{
	return m_KeysPrev[key] != GLFW_PRESS && m_Keys[key] == GLFW_PRESS;
}

bool Input::IsKeyReleased(unsigned int key)
{
	return m_KeysPrev[key] == GLFW_PRESS && m_Keys[key] != GLFW_PRESS;
}
bool Input::IsKeyDown(unsigned int key)
{
	return m_KeysPrev[key] == GLFW_PRESS && m_Keys[key] == GLFW_PRESS;
}

bool Input::IsKeyUp(unsigned int key)
{
	return m_KeysPrev[key] != GLFW_PRESS && m_Keys[key] != GLFW_PRESS;
}

bool Input::IsMouseTriggered(unsigned int key)
{
	return m_ButtonsPrev[key] != GLFW_PRESS && m_Buttons[key] == GLFW_PRESS;
}

bool Input::IsMouseReleased(unsigned int key)
{
	return m_ButtonsPrev[key] == GLFW_PRESS && m_Buttons[key] != GLFW_PRESS;
}
bool Input::IsMouseDown(unsigned int key)
{
	return m_ButtonsPrev[key] == GLFW_PRESS && m_Buttons[key] == GLFW_PRESS;
}

bool Input::IsMouseUp(unsigned int key)
{
	return m_ButtonsPrev[key] != GLFW_PRESS && m_Buttons[key] != GLFW_PRESS;
}