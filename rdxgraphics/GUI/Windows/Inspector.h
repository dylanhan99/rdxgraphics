#pragma once
#include "GUI/BaseGUIWindow.h"
#include "ECS/Components.h"

class Inspector : public BaseGUIWindow
{
public:
	inline Inspector(std::string name, ImGuiWindowFlags flags) : BaseGUIWindow(name, flags) {}
	void UpdateImpl(float dt) override;

#define _RX_X(Klass) void UpdateComp##Klass(std::string const& strHandle, Klass& comp);
	RX_DO_MAIN_COMPONENTS;
#undef _RX_X

#define _RX_X(Klass) void UpdateComp##Klass##BV(std::string const& strHandle, Klass##BV& comp);
	RX_DO_ALL_BV_ENUM;
#undef _RX_X
private:

};