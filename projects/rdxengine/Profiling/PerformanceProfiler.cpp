#include "PerformanceProfiler.h"

using namespace rdx;
namespace ch = std::chrono;

PerformanceProfilerEnterRAII::PerformanceProfilerEnterRAII(std::string name)
{
	PerformanceProfiler::EnterChild(name);
}

PerformanceProfilerEnterRAII::~PerformanceProfilerEnterRAII()
{
	PerformanceProfiler::ExitChild();
}

PerformanceProfilerLogRAII::PerformanceProfilerLogRAII(std::string name)
	: m_Name(std::move(name)), m_StartTime(Now())
{
}

PerformanceProfilerLogRAII::~PerformanceProfilerLogRAII()
{
	PerformanceProfiler::Log(m_Name, m_StartTime, Now());
}

uint64_t PerformanceProfilerLogRAII::Now()
{
	return std::chrono::duration_cast<ch::microseconds>(
		ch::system_clock::now().time_since_epoch()
	).count();
}

PerformanceProfiler::PerformanceProfiler()
	: m_CurrentNode(&m_RootNode)
{
	
}

void PerformanceProfiler::EnterChild(std::string const& name)
{
	auto& g = *Get();
	auto& children = g.m_CurrentNode->Children;
	auto& child = children[name]; // Auto constructs in place if it doesn't exist yet

	// Setting up child
	child.Parent = g.m_CurrentNode;

	// Setting new current node
	g.m_CurrentNode = &child;
}

void PerformanceProfiler::ExitChild()
{
	auto& g = *Get();
	RX_ASSERT(g.m_CurrentNode != &g.m_RootNode);

	g.m_CurrentNode = g.m_CurrentNode->Parent;
}

void PerformanceProfiler::Log(std::string const& name, uint64_t const start, uint64_t const end)
{
	auto& g = *Get();
	RX_ASSERT(g.m_CurrentNode);

	g.m_CurrentNode->Data.emplace_back(NodeData{
			.Name = name,
			.StartTime = start,
			.Duration = end - start
		});
}

PerformanceProfiler::Node const& PerformanceProfiler::GetRootNode()
{ 
	auto& g = *Get();
	return g.m_RootNode; 
}