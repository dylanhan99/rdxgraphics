#ifndef INPUT_H
#define INPUT_H
#include "InputCodes.h"

namespace rdx
{
	class Input
	{
	public:
		void SwapKeys();

		void OnKeyPress(KeyCode const);
		void OnKeyRelease(KeyCode const);

		bool IsKeyTriggered(KeyCode const);
		bool IsKeyReleased(KeyCode const);
		bool IsKeyDown(KeyCode const);
		bool IsKeyUp(KeyCode const);

	private:
		std::array<bool, (size_t)KeyCode::MAX> m_KeyStatesPrev{};
		std::array<bool, (size_t)KeyCode::MAX> m_KeyStatesCurr{};
		std::set<KeyCode> m_KeysTouched{};
	};
}

#endif