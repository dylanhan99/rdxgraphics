#pragma once
#include "GUI/BaseGUIWindow.h"

class EntityHierarchy : public BaseGUIWindow
{
public:
	inline EntityHierarchy(std::string name, ImGuiWindowFlags flags) : BaseGUIWindow(name, flags) {}
	void UpdateImpl(float dt) override;

private:

};