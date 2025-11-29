#include "EngineProfiler.h"
#include "rdxengine/Profiling/PerformanceProfiler.h"

float values_getter(void* data, int index)
{
	using namespace rdx;
	using PP = PerformanceProfiler;

	PP::NodeData const* pData = static_cast<PP::NodeData const*>(data);
	return static_cast<float>(pData[index].Duration);
}

void EngineProfiler::UpdatePanel(float dt)
{
	//float arr[]{ 5.f, 6.f, 1.f, 2.f, 9.f, 15.f, 0.f };
	//ImGui::PlotLines("Performance", arr, IM_ARRAYSIZE(arr), 0, (const char*)0, 3.4028235E38F, 3.4028235E38F, { 0.f, 0.f }, 4);
	{
		using namespace rdx;
		using PP = PerformanceProfiler;
		PP::Node const& rootNode = PP::GetRootNode();

		auto const& data = rootNode.Data;
		void const* pData = data.data();
		int dataLen = std::min(60, static_cast<int>(data.size()));
		int dataOffset = static_cast<int>(data.size()) - dataLen;

		// Stride is handled in the values_getter helper function
		ImGui::PlotLines("Performance", values_getter, (void*)pData, dataLen, dataOffset,
			(const char*)0, 3.4028235E38F, 3.4028235E38F,
			{ 0.f, 300.f });
	}
}
