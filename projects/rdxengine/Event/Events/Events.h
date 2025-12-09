#ifndef EVENTS_H
#define EVENTS_H

namespace rdx
{
	struct ShutdownEngineEvent
	{
		std::string Reason;
	};

	struct FPSChangedEvent
	{
		uint32_t TargetFPS;
	};

	struct ToggleVSyncEvent
	{
		bool IsVSync;
	};

	struct ChangeWindowTitleEvent
	{
		std::string Title;
	};

	// Profiling
	struct TemporaryProfilingEvent
	{
		float ProfilingDuration;
	};

	struct TogglePerformanceProfileEvent
	{
		bool IsAlwaysProfiling;
	};

	//
	struct CameraChangeEvent
	{
		CameraComponent* pCamera;
	};
}

#endif