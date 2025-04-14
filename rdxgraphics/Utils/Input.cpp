#include <pch.h>
#include "Input.h"

RX_SINGLETON_EXPLICIT(Input)

void Input::Init()
{
	EventDispatcher<int, int>::RegisterEvent(RX_EVENT_WINDOW_RESIZE, Input::WindowSizeCallback);
	EventDispatcher<int, int>::RegisterEvent(RX_EVENT_CURSOR_POS_CALLBACK, Input::CursorPosCallback);
}

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

	g.m_KeysPrev[key] = g.m_Keys[key];
	g.m_Keys[key] = action;
}

void Input::ButtonCallback(int btn, int action)
{
	if (btn == GLFW_KEY_UNKNOWN)
		return;

	g.m_ButtonsPrev[btn] = g.m_Buttons[btn];
	g.m_Buttons[btn] = action;
}

void Input::CursorPosCallback(int xpos, int ypos)
{
	g.m_CursorPos.x = xpos;
	g.m_CursorPos.y = ypos;

}

void Input::WindowPosCallback(int xpos, int ypos)
{
	g.m_WindowPos.x = xpos;
	g.m_WindowPos.y = ypos;
}

void Input::WindowSizeCallback(int width, int height)
{
	g.m_WindowDims.x = width;
	g.m_WindowDims.y = height;
}

bool Input::IsKeyTriggered(unsigned int key)
{
	return g.m_KeysPrev[key] != GLFW_PRESS && g.m_Keys[key] == GLFW_PRESS;
}

bool Input::IsKeyReleased(unsigned int key)
{
	return g.m_KeysPrev[key] == GLFW_PRESS && g.m_Keys[key] != GLFW_PRESS;
}
bool Input::IsKeyDown(unsigned int key)
{
	return g.m_KeysPrev[key] == GLFW_PRESS && g.m_Keys[key] == GLFW_PRESS;
}

bool Input::IsKeyUp(unsigned int key)
{
	return g.m_KeysPrev[key] != GLFW_PRESS && g.m_Keys[key] != GLFW_PRESS;
}

bool Input::IsMouseTriggered(unsigned int key)
{
	return g.m_ButtonsPrev[key] != GLFW_PRESS && g.m_Buttons[key] == GLFW_PRESS;
}

bool Input::IsMouseReleased(unsigned int key)
{
	return g.m_ButtonsPrev[key] == GLFW_PRESS && g.m_Buttons[key] != GLFW_PRESS;
}
bool Input::IsMouseDown(unsigned int key)
{
	return g.m_ButtonsPrev[key] == GLFW_PRESS && g.m_Buttons[key] == GLFW_PRESS;
}

bool Input::IsMouseUp(unsigned int key)
{
	return g.m_ButtonsPrev[key] != GLFW_PRESS && g.m_Buttons[key] != GLFW_PRESS;
}