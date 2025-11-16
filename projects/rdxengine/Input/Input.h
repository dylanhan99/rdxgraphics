#ifndef INPUT_H
#define INPUT_H
#include "InputCodes.h"

namespace rdx
{
	class Input
	{
	public:
		void OnKeyPress(KeyCode const);
		void OnKeyRelease(KeyCode const);

		bool IsKeyPress(KeyCode const);

	private:
		std::array<bool, (size_t)KeyCode::MAX> m_KeyStatesPrev{};
		std::array<bool, (size_t)KeyCode::MAX> m_KeyStatesCurr{};
		std::set<KeyCode> m_KeysTouched{};
	};
}

#endif