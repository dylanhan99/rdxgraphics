#ifndef EVENTS_H
#define EVENTS_H

namespace rdx
{
	struct ShutdownEngineEvent
	{
		std::string Reason;
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
}

#endif