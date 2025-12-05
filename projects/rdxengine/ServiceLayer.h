#ifndef RDXENGINE_H
#define RDXENGINE_H
#include "Window/BaseWindow.h"
#include "Graphics/BaseRenderer.h"

#include "Input/Input.h"
#include "Logging/BaseLogger.h"
#include "Event/Busses/InstantEventBus.h"
#include "ECS/BaseEntityComponentWorld.h"
#include "Profiling/PerformanceProfiler.h"
#include "Utils/FrameRateController.h"

#include "BaseApp.h"

namespace rdx
{
	class RX_API ServiceLayer
	{
	public:
		ServiceLayer();

		static void SetServiceLayer(ServiceLayer*);
		static ServiceLayer* GetServiceLayer();

		static BaseWindow* const WindowSystem();
		static BaseRenderer* const RenderingSystem();

		static Input* const InputService();
		static BaseLogger* const LoggingService();
		static InstantEventBus* const InstantEventService();
		static BaseEntityComponentWorld* const EntityComponentService();
		static PerformanceProfiler* const PerformanceProfilingService();
		static FrameRateController* const FrameRateControllerService();

		static BaseApp* const ApplicationService();

		template <typename T>
		void RegisterSystem(T* const p)
		{
			static_assert(std::is_base_of_v<BaseSystem, T>, "T must be derived from BaseSystem");
			RX_ASSERT(p);
			RX_ASSERT(p->s_SystemType < SystemType::MAX);
			RX_ASSERT_MSG(!m_Systems[static_cast<size_t>(p->s_SystemType)], "Already registered type T");

			m_Systems[static_cast<size_t>(p->s_SystemType)] = p;
		}

		template <typename T>
		void RegisterUtil(T* const p)
		{
			static_assert(std::is_base_of_v<BaseUtil, T>, "T must be derived from BaseUtil");
			RX_ASSERT(p);
			RX_ASSERT(p->s_UtilType < UtilType::MAX);
			RX_ASSERT_MSG(!m_Utils[static_cast<size_t>(p->s_UtilType)], "Already registered type T");
			
			m_Utils[static_cast<size_t>(p->s_UtilType)] = p;
		}

		template <typename T>
		void RegisterApp(T* const p)
		{
			static_assert(std::is_base_of_v<BaseApp, T>, "T must be derived from BaseApp");
			RX_ASSERT(p);
			RX_ASSERT_MSG(!m_App, "Already registered App");
			
			m_App = p;
		}

		static std::array<BaseSystem*, static_cast<size_t>(SystemType::MAX)>& GetSystems();
		static std::array<BaseUtil*, static_cast<size_t>(UtilType::MAX)>& GetUtils();
		static BaseApp* GetApp();

	private:
		static ServiceLayer* s_ServiceLayer;

		std::array<BaseSystem*, static_cast<size_t>(SystemType::MAX)> m_Systems{};
		std::array<BaseUtil*, static_cast<size_t>(UtilType::MAX)> m_Utils{};
		BaseApp* m_App{};
	};
}

#endif