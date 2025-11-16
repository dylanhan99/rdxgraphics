#include "Input.h"

using namespace rdx;

void Input::SwapKeys()
{
	std::erase_if(m_KeysTouched,
		[&](KeyCode const& key)
		{
			bool& prev = m_KeyStatesPrev[(size_t)key];
			bool& curr = m_KeyStatesCurr[(size_t)key];
			prev = curr; // Set prev to curr
			return !prev && !curr; // If both are false, we can finally remove from keys touched.
		});

	// Set prev to curr
	//for (KeyCode const key : m_KeysTouched)
	//	m_KeyStatesPrev[(size_t)key] = m_KeyStatesCurr[(size_t)key];
	//m_KeysTouched.clear();
}

void Input::OnKeyPress(KeyCode const key)
{
	m_KeysTouched.emplace(key);
	m_KeyStatesCurr[(size_t)key] = true;
}

void Input::OnKeyRelease(KeyCode const key)
{
	m_KeyStatesCurr[(size_t)key] = false;
}

bool Input::IsKeyTriggered(KeyCode const key)
{
	return !m_KeyStatesPrev[(size_t)key] && m_KeyStatesCurr[(size_t)key];
}

bool Input::IsKeyReleased(KeyCode const key)
{
	return m_KeyStatesPrev[(size_t)key] && !m_KeyStatesCurr[(size_t)key];
}
bool Input::IsKeyDown(KeyCode const key)
{
	return m_KeyStatesPrev[(size_t)key] && m_KeyStatesCurr[(size_t)key];
}

bool Input::IsKeyUp(KeyCode const key)
{
	return !m_KeyStatesPrev[(size_t)key] && !m_KeyStatesCurr[(size_t)key];
}