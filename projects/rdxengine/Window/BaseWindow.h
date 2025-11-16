#ifndef BASEWINDOW_H
#define BASEWINDOW_H
#include "RXAPI.h"
#include "Input/InputCodes.h"

namespace rdx
{
	class RX_API BaseWindow
	{
	public:
		BaseWindow();
		virtual ~BaseWindow();

		virtual bool Init() = 0;
		virtual bool Terminate() = 0;
		void PollEvents();

		virtual bool IsWindowShouldClose() = 0;
		virtual void SetShouldClose() = 0;

	private:
		virtual void PollEventsImpl() = 0;

	protected:
		void OnKeyPress(KeyCode const);
		void OnKeyRelease(KeyCode const);
	};
}

#endif