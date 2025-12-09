#ifndef HIERARCHY_H
#define HIERARCHY_H
#include "GUI/BasePanel.h"

class Hierarchy final : public BasePanel
{
public:
	Hierarchy(const char* windowName, ImGuiWindowFlags flags) : BasePanel(windowName, flags) {}

private:
	void UpdateImpl(float dt) override;
};

#endif