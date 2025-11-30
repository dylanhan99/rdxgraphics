#ifndef RDXENGINE_H
#define RDXENGINE_H
#include "Window/BaseWindow.h"
#include "Input/Input.h"
#include "Logging/BaseLogger.h"
#include "Event/Busses/InstantEventBus.h"
#include "ECS/BaseEntityComponentWorld.h"
#include "Graphics/BaseRenderer.h"
#include "Profiling/PerformanceProfiler.h"
#include "BaseApp.h"

namespace rdx
{
	class RX_API ServiceLayer
	{
	public:
		ServiceLayer(BaseWindow*, Input*, BaseLogger*, InstantEventBus*, BaseEntityComponentWorld*, BaseRenderer*, BaseApp*, PerformanceProfiler*);
		static void SetServiceLayer(ServiceLayer*);
		static ServiceLayer* GetServiceLayer();

		static BaseWindow* const WindowService();
		static Input* const InputService();
		static BaseLogger* const LoggingService();
		static InstantEventBus* const InstantEventService();
		static BaseEntityComponentWorld* const EntityComponentService();
		static BaseRenderer* const RenderingService();
		static BaseApp* const ApplicationService();
		static PerformanceProfiler* const PerformanceProfilingService();

	private:
		static ServiceLayer* s_ServiceLayer;

		BaseWindow* m_WindowService{};
		Input* m_InputService{};
		BaseLogger* m_LoggingService{};
		InstantEventBus* m_InstantEventService{};
		BaseEntityComponentWorld* m_EntityComponentService{};
		BaseRenderer* m_RenderingService{};
		BaseApp* m_ApplicationService{};
		PerformanceProfiler* m_PerformanceProfilingService{};
	};
}

#endif