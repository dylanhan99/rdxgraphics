#ifndef BASEWINDOW_H
#define BASEWINDOW_H
#include "RXAPI.h"
#include "Input/InputCodes.h"
#include "BaseSystem.h"

namespace rdx
{
	class RX_API BaseWindow : public BaseSystem
	{
		RX_DECLARE_SYSTEM("Window", Window);
	public:
		virtual ~BaseWindow();

		virtual bool InitImpl() = 0;
		virtual bool TerminateImpl() = 0;
		void PollEvents();
		virtual void SwapBuffers() = 0;

		virtual bool IsWindowShouldClose() = 0;
		virtual void SetShouldClose() = 0;

		virtual glm::vec2 GetWindowDims() const { return {}; }
		virtual void SetMousePos(int, int) const {}

	private:
		virtual void PollEventsImpl() = 0;

	protected:
		void OnKeyPress(KeyCode const);
		void OnKeyRelease(KeyCode const);
		void OnMousePress(MouseCode const);
		void OnMouseRelease(MouseCode const);
		void OnMouseMove(double const xpos, double const ypos);
		void OnScroll(double const yoffset);
	};
}

#endif