#include "FrameRateController.h"
#include "ServiceLayer.h"
#include "Event/Events/Events.h"

namespace ch = std::chrono;
using namespace rdx;

bool FrameRateController::InitImpl()
{
	m_PrevTargetFrames = m_TargetFrames; // Default prev to whatever target is
	m_TargetDT = 1.f / m_TargetFrames;
	m_LastTime = ch::system_clock::now();

	ServiceLayer::InstantEventService()->Subscribe<FPSChangedEvent>(
		[this](FPSChangedEvent const& e)
		{
			m_TargetFrames = e.TargetFPS;
			m_TargetDT = 1.f / m_TargetFrames;
		});
	ServiceLayer::InstantEventService()->Subscribe<ToggleVSyncEvent>(
		[this](ToggleVSyncEvent const& e)
		{
			m_IsVSync = e.IsVSync;

			m_PrevTargetFrames = e.IsVSync ? m_TargetFrames : m_PrevTargetFrames;
			m_TargetFrames = e.IsVSync ? std::numeric_limits<int>::max() : m_PrevTargetFrames;
			m_TargetDT = 1.f / m_TargetFrames;
		});

	return true;
}

bool FrameRateController::TerminateImpl()
{
	return true;
}

void FrameRateController::FrameStart()
{
	++m_FrameCount;

	{
		auto now = ch::system_clock::now();
		auto elapsed = ch::duration_cast<ch::milliseconds>(now - m_LastTime);
		m_LastTime = now;
		m_DT = elapsed.count() * 0.001f; // Convert ms to seconds
	}

	{ // FPS value displayer. Resets every 1 second
		static float dtAccum = 0.f;
		static int numFrames = 0;

		dtAccum += m_DT;
		++numFrames;

		if (dtAccum >= 1.f)
		{
			m_EstimatedFPS = numFrames;
			dtAccum = 0.f;
			numFrames = 0;
		}
	}
}

void FrameRateController::FrameEnd()
{
	if (!IsVSync())
	{
		// Becareful of a pure sleep_until approach. The OS may sleep for +-[2,5]ms, which is inconsequential at lower fps
		// But at higher target fps, the 2ms really adds up, maybe a 244fps may only perform at 150+fps
		// Not really important right now, but something to thinka bout next time.
		auto end = m_LastTime + ch::round<ch::milliseconds>(ch::duration<float>(m_TargetDT)); // The "correct" next frame time
		std::this_thread::sleep_until(end);
	}
}