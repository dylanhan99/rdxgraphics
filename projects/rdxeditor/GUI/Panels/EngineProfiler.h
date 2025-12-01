#ifndef ENGINEPROFILER_H
#define ENGINEPROFILER_H
#include "GUI/BasePanel.h"

class EngineProfiler : public BasePanel
{
public:
	EngineProfiler(const char* windowName, ImGuiWindowFlags flags = 0) : BasePanel(windowName, flags) {}

private:
	void UpdateImpl(float dt) override final;
};

#endif