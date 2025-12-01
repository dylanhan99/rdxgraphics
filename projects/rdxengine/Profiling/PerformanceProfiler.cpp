#include "PerformanceProfiler.h"
#include "ServiceLayer.h"

using namespace rdx;
namespace ch = std::chrono;

PerformanceProfilerFrameRAII::PerformanceProfilerFrameRAII(uint64_t const frameNumber)
	: m_StartTime(Now())
{
	auto* const pp = ServiceLayer::PerformanceProfilingService();
	if (pp->IsProfiling())
		pp->BeginFrame(frameNumber);
}

PerformanceProfilerFrameRAII::~PerformanceProfilerFrameRAII()
{
	auto* const pp = ServiceLayer::PerformanceProfilingService();
	if (pp->IsProfiling())
		pp->EndFrame(m_StartTime, Now());
}

PerformanceProfilerLogRAII::PerformanceProfilerLogRAII(std::string name)
	: m_StartTime(PerformanceProfilerFrameRAII::Now())
{
	auto* const pp = ServiceLayer::PerformanceProfilingService();
	if (pp->IsProfiling())
		pp->BeginChild(std::move(name));
}

PerformanceProfilerLogRAII::~PerformanceProfilerLogRAII()
{
	auto* const pp = ServiceLayer::PerformanceProfilingService();
	if (pp->IsProfiling())
		pp->EndChild(m_StartTime, PerformanceProfilerFrameRAII::Now());
}

uint64_t PerformanceProfilerFrameRAII::Now()
{
	return std::chrono::duration_cast<ch::microseconds>(
		ch::system_clock::now().time_since_epoch()
	).count();
}

PerformanceProfiler::PerformanceProfiler()
{
}

void PerformanceProfiler::BeginFrame(uint64_t const frameNumber)
{
	Node newNode{};
	newNode.Name = "Frame: " + std::to_string(frameNumber);


	m_Frames.emplace_back(std::move(Frame{
		.NodeData = std::move(newNode),
		.FrameNumber = frameNumber
	}));

	m_CurrentNode = &m_Frames.back().NodeData;
}

void PerformanceProfiler::EndFrame(uint64_t const start, uint64_t const end)
{
	auto& frame = m_Frames.back();
	auto& node = frame.NodeData;
	node.StartTime = start;
	node.Duration = end - start;
}

void PerformanceProfiler::BeginChild(std::string name)
{
	RX_ASSERT(m_CurrentNode);

	auto& childs = m_CurrentNode->Children;
	Node newNode{};
	newNode.Name = std::move(name);
	newNode.Parent = m_CurrentNode;
	childs.emplace_back(std::move(newNode));

	// Set current node to the most recent child
	m_CurrentNode = &childs.back();
}

void PerformanceProfiler::EndChild(uint64_t const start, uint64_t const end)
{
	RX_ASSERT(m_CurrentNode);

	auto& cnode = *m_CurrentNode;
	cnode.StartTime = start;
	cnode.Duration = end - start;

	m_CurrentNode = m_CurrentNode->Parent;
	if (!m_CurrentNode)
		RX_WARN("m_CurrentNode is NULL");
}

std::vector<PerformanceProfiler::Frame> const& PerformanceProfiler::GetFrames() const
{ 
	return m_Frames; 
}

bool PerformanceProfiler::IsProfiling() const
{
	return IsAlwaysProfiling() || m_RecordingTime < m_RecordingDuration;
}

bool PerformanceProfiler::IsAlwaysProfiling() const
{
	return m_IsAlwaysProfiling;
}

void PerformanceProfiler::EnableProfiling()
{
	m_RecordingTime = m_RecordingDuration;
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
		m_RecordingTime -= dt;
		if (IsAlwaysProfiling())
		{ // Clamp the size to target fps
			if (m_Frames.size() > m_TargetFramerate)
			{
				const size_t toRemove = m_Frames.size() - m_TargetFramerate;
				auto it = m_Frames.begin();
				std::advance(it, toRemove);
				m_Frames.erase(m_Frames.begin(), it);
			}
		}
		else if (m_RecordingTime <= 0.f)
		{
			RX_DEBUG("Profiling Ended");
			m_Frames.clear();
		}
	}
}
