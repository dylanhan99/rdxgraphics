#pragma once
#include "GUI/BaseGUIWindow.h"
#include "ECS/Components.h"

class Inspector : public BaseGUIWindow
{
	RX_GUIWINDOW_DEC(Inspector);
public:
#define _RX_X(Klass) void UpdateComp##Klass(std::string const& strHandle, Klass& comp);
	RX_DO_MAIN_COMPONENTS;
#undef _RX_X

#define _RX_X(Klass) void UpdateComp##Klass##Primitive(std::string const& strHandle, Klass##Primitive& comp);
	RX_DO_ALL_BV_ENUM;
#undef _RX_X
};