#ifndef INPUT_H
#define INPUT_H
#include "RXAPI.h"
#include "BaseUtil.h"
#include "InputCodes.h"

namespace rdx
{
	class RX_API Input final : public BaseUtil
	{
		RX_DECLARE_UTIL("Input", Input)
	public:
		inline bool InitImpl() { return true; }
		inline bool TerminateImpl() { return true; }

		void SwapKeys();

		void OnKeyPress(KeyCode const);
		void OnKeyRelease(KeyCode const);
		void OnMousePress(MouseCode const);
		void OnMouseRelease(MouseCode const);
		void OnMouseMove(glm::vec2);
		void OnScroll(float const);

		bool IsKeyTriggered(KeyCode const) const;
		bool IsKeyReleased(KeyCode const) const;
		bool IsKeyDown(KeyCode const) const;
		bool IsKeyUp(KeyCode const) const;

		bool IsMouseTriggered(MouseCode const) const;
		bool IsMouseReleased(MouseCode const) const;
		bool IsMouseDown(MouseCode const) const;
		bool IsMouseUp(MouseCode const) const;

		glm::vec2 GetMousePosDelta() const { return m_MousePosCurr - m_MousePosPrev; }
		glm::vec2 const& GetMousePos() const { return m_MousePosCurr; }
		void SetMousePos(glm::vec2 pos) const;

		float GetScrollDelta() const { return m_ScrollDelta; }

	private:
		std::array<bool, (size_t)KeyCode::MAX> m_KeyStatesPrev{};
		std::array<bool, (size_t)KeyCode::MAX> m_KeyStatesCurr{};
		std::array<bool, (size_t)MouseCode::MAX> m_MouseStatesPrev{};
		std::array<bool, (size_t)MouseCode::MAX> m_MouseStatesCurr{};
		std::set<KeyCode> m_KeysTouched{};
		std::set<MouseCode> m_MouseTouched{};

		glm::vec2 m_MousePosPrev{}; // Window space
		glm::vec2 m_MousePosCurr{}; 

		float m_ScrollDelta{}; // +ve up, -ve down
	};
}

#endif