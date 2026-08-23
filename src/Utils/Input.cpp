#include "Input.h"

RX_SINGLETON_EXPLICIT(Input)

void Input::Init()
{
}

void Input::SwapKeys()
{
	std::ranges::copy(g.m_Keys.begin(), g.m_Keys.end(), g.m_KeysPrev.begin());
	std::ranges::copy(g.m_Buttons.begin(), g.m_Buttons.end(), g.m_ButtonsPrev.begin());
	g.m_ScrollOffset = 0.0;
}

void Input::OnInputEvent(const RawKeyInputEvent& e)
{
	if (e.key == RX_KEY_UNKNOWN)
		return;

	g.m_KeysPrev[e.key] = g.m_Keys[e.key];
	g.m_Keys[e.key] = e.action;
}

void Input::OnInputEvent(const RawButtonInputEvent& e)
{
	if (e.button == RX_KEY_UNKNOWN)
		return;

	g.m_ButtonsPrev[e.button] = g.m_Buttons[e.button];
	g.m_Buttons[e.button] = e.action;
}

void Input::OnInputEvent(const RawCursorMovedEvent& e)
{
	//g.m_MousePos.x = xpos;
	//g.m_MousePos.y = ypos;
}

void Input::OnInputEvent(const RawScrollEvent& e)
{
	g.m_ScrollOffset = e.yoffset;
}

bool Input::IsKeyTriggered(unsigned int key)
{
	return g.m_KeysPrev[key] != RX_PRESS && g.m_Keys[key] == RX_PRESS;
}

bool Input::IsKeyReleased(unsigned int key)
{
	return g.m_KeysPrev[key] == RX_PRESS && g.m_Keys[key] != RX_PRESS;
}
bool Input::IsKeyDown(unsigned int key)
{
	return g.m_KeysPrev[key] == RX_PRESS && g.m_Keys[key] == RX_PRESS;
}

bool Input::IsKeyUp(unsigned int key)
{
	return g.m_KeysPrev[key] != RX_PRESS && g.m_Keys[key] != RX_PRESS;
}

bool Input::IsMouseTriggered(unsigned int key)
{
	return g.m_ButtonsPrev[key] != RX_PRESS && g.m_Buttons[key] == RX_PRESS;
}

bool Input::IsMouseReleased(unsigned int key)
{
	return g.m_ButtonsPrev[key] == RX_PRESS && g.m_Buttons[key] != RX_PRESS;
}
bool Input::IsMouseDown(unsigned int key)
{
	return g.m_ButtonsPrev[key] == RX_PRESS && g.m_Buttons[key] == RX_PRESS;
}

bool Input::IsMouseUp(unsigned int key)
{
	return g.m_ButtonsPrev[key] != RX_PRESS && g.m_Buttons[key] != RX_PRESS;
}
