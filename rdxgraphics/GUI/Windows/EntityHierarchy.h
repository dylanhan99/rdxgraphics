#pragma once
#include "GUI/BaseGUIWindow.h"
#include "GSM/BaseScene.h"

class EntityHierarchy : public BaseGUIWindow
{
	RX_GUIWINDOW_DEC(EntityHierarchy);
private:
	static void UpdateScene(std::shared_ptr<BaseScene> pScene, int id);
};