#ifndef PERFORMANCEPROFILER_H
#define PERFORMANCEPROFILER_H
#include "BaseUtil.h"

#define RX_PROFILE_FRAME(frame) rdx::PerformanceProfilerFrameRAII(frame)
#define RX_PROFILE(name) rdx::PerformanceProfilerLogRAII(name)
#define RX_PROFILE_FUNC() RX_PROFILE(__func__)

namespace rdx
{
	class PerformanceProfilerFrameRAII
	{
	public:
		PerformanceProfilerFrameRAII(uint64_t const frameNumber);
		~PerformanceProfilerFrameRAII();

	public:
		static uint64_t Now();

	private:
		uint64_t m_StartTime{};
	};

	class PerformanceProfilerLogRAII
	{
	public:
		PerformanceProfilerLogRAII(std::string name);
		~PerformanceProfilerLogRAII();

	private:
		uint64_t m_StartTime{};
	};

	class PerformanceProfiler : public BaseUtil
	{
		RX_DECLARE_UTIL("Performance Profiler", PerformanceProfiler)
	public:
		struct Node {
			std::vector<Node> Children{};
			std::string Name{};
			uint64_t StartTime{};
			uint64_t Duration{};
			Node* Parent{};
		};

		struct Frame {
			Node NodeData;
			uint64_t FrameNumber;
		};

	public:
		PerformanceProfiler();

		void BeginFrame(uint64_t const frameNumber);
		void EndFrame(uint64_t const start, uint64_t const end);
		void BeginChild(std::string name);
		void EndChild(uint64_t const start, uint64_t const end);

		std::vector<Frame> const& GetFrames() const;
		bool IsProfiling() const;
		bool IsAlwaysProfiling() const;
		void EnableProfiling();

		void Update(float dt);

	private:
		bool InitImpl() override;
		bool TerminateImpl() override;

	private:
		std::vector<Frame> m_Frames{};
		Node* m_CurrentNode{};

		float m_RecordingDuration{ 5.f };
		float m_RecordingTime{ 0.f };
		bool m_IsAlwaysProfiling{ true };
		uint32_t m_TargetFramerate{ 60 }; // Used to limit the size of maps when IsAlwaysEnabled
	};
}

#endif