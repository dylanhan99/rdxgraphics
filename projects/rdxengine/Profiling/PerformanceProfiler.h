#ifndef PERFORMANCEPROFILER_H
#define PERFORMANCEPROFILER_H

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

	class PerformanceProfiler
	{
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
		static void EnterChild(std::string const& name);
		static void ExitChild();
		static void Log(std::string const& name, uint64_t const start, uint64_t const end);

		static Node const& GetRootNode();

	private:
		PerformanceProfiler();
		inline static PerformanceProfiler* Get() {
			static PerformanceProfiler g{};
			return &g;
		}

	private:
		Node m_RootNode{};
		Node* m_CurrentNode{};
	};
}

#endif