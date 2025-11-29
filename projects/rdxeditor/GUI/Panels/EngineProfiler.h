#ifndef ENGINEPROFILER_H
#define ENGINEPROFILER_H
#include "GUI/BasePanel.h"

class EngineProfiler : public BasePanel
{
public:
	EngineProfiler(const char* windowName, ImGuiWindowFlags flags = 0) : BasePanel(windowName, flags) {}

private:
	inline bool InitImpl() override { return true; }
	inline bool TerminateImpl() override { return true; }

	void UpdatePanel(float dt) override final;
};

#endif