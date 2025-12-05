#ifndef FRAMERATECONTROLLER_H
#define FRAMERATECONTROLLER_H
#include "BaseUtil.h"

namespace rdx
{
	constexpr uint32_t RX_DEFAULT_TARGET_FRAMES = 30;

	class FrameRateController : public BaseUtil
	{
		RX_DECLARE_UTIL("FrameRate Controller ", FrameRateController)
	public:
		FrameRateController(){}

		void FrameStart();
		void FrameEnd();

		inline uint32_t GetEstimatedFPS() const { return m_EstimatedFPS; }
		inline float GetDT() const { return m_DT; }
		inline float GetFrame() const { return m_FrameCount; }
		inline bool IsVSync() const { return m_IsVSync; }

	private:
		bool InitImpl() override;
		bool TerminateImpl() override;

	private:
		uint64_t m_FrameCount{};

		std::chrono::time_point<std::chrono::system_clock>
			m_LastTime{};

		uint32_t m_EstimatedFPS{};
		uint32_t m_TargetFrames{ RX_DEFAULT_TARGET_FRAMES };
		float m_TargetDT{};
		float m_DT{};

		bool m_IsVSync{ true };
		uint32_t m_PrevTargetFrames{}; // Cache value whenever isVsync is changed
	};
}

#endif