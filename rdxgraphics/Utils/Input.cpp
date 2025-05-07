#include <pch.h>
#include "Input.h"

RX_SINGLETON_EXPLICIT(Input)

void Input::Init()
{
}

void Input::SwapKeys()
{
	std::ranges::copy(g.m_Keys.begin(), g.m_Keys.end(), g.m_KeysPrev.begin());
	std::ranges::copy(g.m_Buttons.begin(), g.m_Buttons.end(), g.m_ButtonsPrev.begin());
}

void Input::KeyCallback(int key, int scancode, int action)
{
	RX_UNREF_PARAM(scancode);

	if (key == RX_KEY_UNKNOWN)
		return;

	g.m_KeysPrev[key] = g.m_Keys[key];
	g.m_Keys[key] = action;
}

void Input::ButtonCallback(int btn, int action)
{
	if (btn == RX_KEY_UNKNOWN)
		return;

	g.m_ButtonsPrev[btn] = g.m_Buttons[btn];
	g.m_Buttons[btn] = action;
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