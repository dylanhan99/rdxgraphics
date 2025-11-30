#ifndef PERFORMANCEPROFILER_H
#define PERFORMANCEPROFILER_H
#include "BaseUtil.h"

#define RX_PROFILE_ENTER(name) rdx::PerformanceProfilerEnterRAII(name)
#define RX_PROFILE(name) rdx::PerformanceProfilerLogRAII(name)

namespace rdx
{
	class PerformanceProfilerEnterRAII
	{
	public:
		PerformanceProfilerEnterRAII(std::string name);
		~PerformanceProfilerEnterRAII();
	};

	class PerformanceProfilerLogRAII
	{
	public:
		PerformanceProfilerLogRAII(std::string name);
		~PerformanceProfilerLogRAII();

	private:
		static uint64_t Now();

	private:
		std::string m_Name{};
		uint64_t m_StartTime{};
	};

	class PerformanceProfiler : public BaseUtil
	{
		RX_DECLARE_UTIL("Performance Profiler", PerformanceProfiler)
	public:
		struct NodeData {
			std::string Name{};
			uint64_t StartTime{}; // micro seconds
			uint64_t Duration{};  // micro seconds
		};

		struct Node {
			Node* Parent{};
			std::vector<NodeData> Data{};
			std::map<std::string, Node> Children{}; // Don't accidentally overlap profile names
		};

	public:
		PerformanceProfiler();

		void EnterChild(std::string const& name);
		void ExitChild();
		void Log(std::string const& name, uint64_t const start, uint64_t const end);

		Node const& GetRootNode();
		bool IsProfiling() const;
		void EnableProfiling();

		void Update(float dt);

	private:
		bool InitImpl() override;
		bool TerminateImpl() override;

	private:
		Node m_RootNode{};
		Node* m_CurrentNode{};

		inline static const float s_RecordingDuration{ 10.f };
		float m_RecordingTime{ s_RecordingDuration };
	};
}

#endif