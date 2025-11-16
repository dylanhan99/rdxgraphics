#ifndef RDXENGINE_H
#define RDXENGINE_H
#include "Window/BaseWindow.h"
#include "Input/Input.h"

namespace rdx
{
	class RX_API ServiceLayer
	{
	public:
		ServiceLayer(BaseWindow*, Input*);
		static void RegisterServiceLayer(ServiceLayer*);

		static BaseWindow* const WindowService();
		static Input* const InputService();

	private:
		inline static ServiceLayer* s_ServiceLayer{};

		BaseWindow* m_WindowService{};
		Input* m_InputService{};
	};
}

#endif