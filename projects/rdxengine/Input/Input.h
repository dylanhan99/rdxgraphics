#ifndef INPUT_H
#define INPUT_H
#include "RXAPI.h"
#include "BaseService.h"
#include "InputCodes.h"

namespace rdx
{
	class RX_API Input final : public BaseService
	{
	public:
		inline bool InitImpl() { return true; }
		inline bool TerminateImpl() { return true; }

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