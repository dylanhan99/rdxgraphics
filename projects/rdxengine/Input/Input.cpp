#include "Input.h"
#include "ServiceLayer.h"

using namespace rdx;

void Input::SwapKeys()
{
	std::erase_if(m_KeysTouched,
		[this](KeyCode const& key)
		{
			bool& prev = m_KeyStatesPrev[(size_t)key];
			bool& curr = m_KeyStatesCurr[(size_t)key];
			prev = curr; // Set prev to curr
			return !prev && !curr; // If both are false, we can finally remove from keys touched.
		});
	std::erase_if(m_MouseTouched,
		[this](MouseCode const& button)
		{
			bool& prev = m_MouseStatesPrev[(size_t)button];
			bool& curr = m_MouseStatesCurr[(size_t)button];
			prev = curr;
			return !prev && !curr;
		});

	m_MousePosPrev = m_MousePosCurr;
	m_ScrollDelta = 0.f;
}

void Input::OnKeyPress(KeyCode const key)
{
	m_KeysTouched.emplace(key);
	m_KeyStatesCurr[static_cast<size_t>(key)] = true;
}

void Input::OnKeyRelease(KeyCode const key)
{
	m_KeyStatesCurr[static_cast<size_t>(key)] = false;
}

void Input::OnMousePress(MouseCode const button)
{
	m_MouseTouched.emplace(button);
	m_MouseStatesCurr[static_cast<size_t>(button)] = true;
}

void Input::OnMouseRelease(MouseCode const button)
{
	m_MouseStatesCurr[static_cast<size_t>(button)] = false;
}

void Input::OnMouseMove(glm::vec2 mousePos)
{
	m_MousePosCurr = mousePos;
}

void Input::OnScroll(float const yoffset)
{
	m_ScrollDelta = yoffset;
}

bool Input::IsKeyTriggered(KeyCode const key) const
{
	return !m_KeyStatesPrev[static_cast<size_t>(key)] && m_KeyStatesCurr[static_cast<size_t>(key)];
}

bool Input::IsKeyReleased(KeyCode const key) const
{
	return m_KeyStatesPrev[static_cast<size_t>(key)] && !m_KeyStatesCurr[static_cast<size_t>(key)];
}
bool Input::IsKeyDown(KeyCode const key) const
{
	return m_KeyStatesPrev[static_cast<size_t>(key)] && m_KeyStatesCurr[static_cast<size_t>(key)];
}

bool Input::IsKeyUp(KeyCode const key) const
{
	return !m_KeyStatesPrev[static_cast<size_t>(key)] && !m_KeyStatesCurr[static_cast<size_t>(key)];
}

bool Input::IsMouseTriggered(MouseCode const button) const
{
	return !m_MouseStatesPrev[static_cast<size_t>(button)] && m_MouseStatesCurr[static_cast<size_t>(button)];
}

bool Input::IsMouseReleased(MouseCode const button) const
{
	return m_MouseStatesPrev[static_cast<size_t>(button)] && !m_MouseStatesCurr[static_cast<size_t>(button)];
}
bool Input::IsMouseDown(MouseCode const button) const
{
	return m_MouseStatesPrev[static_cast<size_t>(button)] && m_MouseStatesCurr[static_cast<size_t>(button)];
}

bool Input::IsMouseUp(MouseCode const button) const
{
	return !m_MouseStatesPrev[static_cast<size_t>(button)] && !m_MouseStatesCurr[static_cast<size_t>(button)];
}

void Input::SetMousePos(glm::vec2 pos) const
{
	ServiceLayer::WindowSystem()->SetMousePos((int)pos.x, (int)pos.y);
}