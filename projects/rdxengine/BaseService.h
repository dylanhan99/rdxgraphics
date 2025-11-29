#ifndef BASESERVICE_H
#define BASESERVICE_H
#include "Profiling/PerformanceProfiler.h"

namespace rdx
{
	class BaseService
	{
	public:
		virtual ~BaseService() = default;

		inline bool Init()
		{
			RX_PROFILE_ENTER(GetName());
			RX_PROFILE("Initialization");

			if (IsInitialized())
				return false;

			return m_IsInitialized = InitImpl();
		}

		inline bool Terminate()
		{
			RX_PROFILE_ENTER(GetName());
			RX_PROFILE("Terminate");

			bool success = false;
			if (IsInitialized())
				success = TerminateImpl();

			m_IsInitialized = false;
			return success;
		}

		inline void Update(float dt)
		{
			RX_PROFILE_ENTER(GetName());
			RX_PROFILE("Update");

			UpdateImpl(dt);
		}

		inline void Draw()
		{
			RX_PROFILE_ENTER(GetName());
			RX_PROFILE("Draw");

			DrawImpl();
		}

		inline bool IsInitialized() const { return m_IsInitialized; }

		inline virtual const char* GetName() const { return "Missing Name"; }

	private:
		virtual bool InitImpl() = 0;
		virtual bool TerminateImpl() = 0;

		virtual void UpdateImpl(float dt) {};
		virtual void DrawImpl() {};

	private:
		bool m_IsInitialized{ false };
	};
}

#endif