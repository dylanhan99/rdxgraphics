#include "Input.h"

using namespace rdx;

void Input::OnKeyPress(KeyCode const key)
{
	m_KeysTouched.emplace(key);
	m_KeyStatesCurr[(size_t)key] = true;
}

void Input::OnKeyRelease(KeyCode const key)
{
	m_KeysTouched.erase(key);
	m_KeyStatesCurr[(size_t)key] = false; // to remove. Change to swap keys
}

bool Input::IsKeyPress(KeyCode const key)
{
	return m_KeyStatesCurr[(size_t)key];
}