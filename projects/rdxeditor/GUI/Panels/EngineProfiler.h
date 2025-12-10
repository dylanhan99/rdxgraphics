#ifndef ENGINEPROFILER_H
#define ENGINEPROFILER_H
#include "GUI/BasePanel.h"

namespace rdxgui
{
	class EngineProfiler final : public BasePanel
	{
	public:
		EngineProfiler(const char* windowName, ImGuiWindowFlags flags) : BasePanel(windowName, flags) {}

	private:
		void UpdateImpl(float dt) override;
	};
}

#endif