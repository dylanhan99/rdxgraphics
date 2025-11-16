#ifndef RDXENGINE_H
#define RDXENGINE_H
#include "Window/BaseWindow.h"
#include "Input/Input.h"
#include "Logging/AsyncLogger.h"

namespace rdx
{
	class RX_API ServiceLayer
	{
	public:
		ServiceLayer(BaseWindow*, Input*, AsyncLogger*);
		static void RegisterServiceLayer(ServiceLayer*);

		static BaseWindow* const WindowService();
		static Input* const InputService();
		static AsyncLogger* const LoggingService();

	private:
		inline static ServiceLayer* s_ServiceLayer{};

		BaseWindow* m_WindowService{};
		Input* m_InputService{};
		AsyncLogger* m_LoggingService{};
	};
}

#endif