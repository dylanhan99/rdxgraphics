#include "PerformanceProfiler.h"
#include "ServiceLayer.h"

using namespace rdx;
namespace ch = std::chrono;

PerformanceProfilerEnterRAII::PerformanceProfilerEnterRAII(std::string name)
{
	if (ServiceLayer::PerformanceProfilingService()->IsProfiling())
		ServiceLayer::PerformanceProfilingService()->EnterChild(name);
}

PerformanceProfilerEnterRAII::~PerformanceProfilerEnterRAII()
{
	if (ServiceLayer::PerformanceProfilingService()->IsProfiling())
		ServiceLayer::PerformanceProfilingService()->ExitChild();
}

PerformanceProfilerLogRAII::PerformanceProfilerLogRAII(std::string name)
	: m_Name(std::move(name)), m_StartTime(Now())
{
}

PerformanceProfilerLogRAII::~PerformanceProfilerLogRAII()
{
	if (ServiceLayer::PerformanceProfilingService()->IsProfiling())
		ServiceLayer::PerformanceProfilingService()->Log(m_Name, m_StartTime, Now());
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
	auto& children = m_CurrentNode->Children;
	auto& child = children[name]; // Auto constructs in place if it doesn't exist yet

	// Setting up child
	child.Parent = m_CurrentNode;

	// Setting new current node
	m_CurrentNode = &child;
}

void PerformanceProfiler::ExitChild()
{
	RX_ASSERT(m_CurrentNode != &m_RootNode);

	m_CurrentNode = m_CurrentNode->Parent;
}

void PerformanceProfiler::Log(std::string const& name, uint64_t const start, uint64_t const end)
{
	RX_ASSERT(m_CurrentNode);

	m_CurrentNode->Data.emplace_back(NodeData{
			.Name = name,
			.StartTime = start,
			.Duration = end - start
		});
}

PerformanceProfiler::Node const& PerformanceProfiler::GetRootNode()
{ 
	return m_RootNode; 
}

bool PerformanceProfiler::IsProfiling() const
{
	return m_RecordingTime < s_RecordingDuration;
}

void PerformanceProfiler::EnableProfiling()
{
	m_RecordingTime = 0.f;
	RX_DEBUG("Profiling Started");
}

bool PerformanceProfiler::InitImpl()
{
	return true;
}

bool PerformanceProfiler::TerminateImpl()
{
	return true;
}

void PerformanceProfiler::Update(float dt)
{
	if (IsProfiling())
	{
		m_RecordingTime += dt;
		if (m_RecordingTime >= s_RecordingDuration)
		{
			RX_DEBUG("Profiling Ended");
		}
	}
}
