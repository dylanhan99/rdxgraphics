#ifndef RDXENGINE_H
#define RDXENGINE_H
#include "Window/BaseWindow.h"
#include "Input/Input.h"
#include "Logging/BaseLogger.h"
#include "Event/Busses/InstantEventBus.h"
#include "ECS/BaseEntityComponentWorld.h"

namespace rdx
{
	class RX_API ServiceLayer
	{
	public:
		ServiceLayer(BaseWindow*, Input*, BaseLogger*, InstantEventBus*, BaseEntityComponentWorld*);
		static void RegisterServiceLayer(ServiceLayer*);

		static BaseWindow* const WindowService();
		static Input* const InputService();
		static BaseLogger* const LoggingService();
		static InstantEventBus* const InstantEventService();
		static BaseEntityComponentWorld* const EntityComponentService();

	private:
		inline static ServiceLayer* s_ServiceLayer{};

		BaseWindow* m_WindowService{};
		Input* m_InputService{};
		BaseLogger* m_LoggingService{};
		InstantEventBus* m_InstantEventService{};
		BaseEntityComponentWorld* m_EntityComponentService{};
	};
}

#endif