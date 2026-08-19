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

void Input::ScrollCallback(double, double yoffset)
{
	g.m_ScrollOffset = yoffset;
}

void Input::MousePosCallback(double xpos, double ypos)
{
	//g.m_MousePos.x = xpos;
	//g.m_MousePos.y = ypos;
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

void Input::OnInputEvent(const RawKeyInputEvent& e)
{
	KeyCallback(e.key, e.scancode, e.action);

	InputAction action =
		IsKeyTriggered(e.key) ? InputAction::Triggered :
		IsKeyReleased(e.key) ?	InputAction::Released :
		IsKeyDown(e.key) ?		InputAction::Down :
								InputAction::Up;

	EventBus::Raise(KeyInputEvent{
		.key = e.key,
		.action = action
		});
}

void Input::OnInputEvent(const RawButtonInputEvent& e)
{
	ButtonCallback(e.button, e.action);

	InputAction action =
		IsMouseTriggered(e.button) ? InputAction::Triggered :
		IsMouseReleased(e.button) ?  InputAction::Released :
		IsMouseDown(e.button) ?		 InputAction::Down :
									 InputAction::Up;

	EventBus::Raise(MouseInputEvent{
		.button = e.button,
		.action = action
		});
}

void Input::OnInputEvent(const RawCursorMovedEvent& e)
{
	MousePosCallback(e.xpos, e.ypos);

	EventBus::Raise(CursorMovedEvent{
		.position = glm::vec2{ e.xpos, e.ypos }
		});
}

void Input::OnInputEvent(const RawScrollEvent& e)
{
	ScrollCallback(e.xoffset, e.yoffset);

	EventBus::Raise(MouseInputEvent{
		.offset = e.yoffset,
		.action = e.yoffset > 0.0 ? InputAction::ScrolledUp : InputAction::ScrolledDown
		});
}
