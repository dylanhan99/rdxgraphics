#pragma once
#include "GUI/BaseGUIWindow.h"
#include "GSM/BaseScene.h"

class EntityHierarchy : public BaseGUIWindow
{
	RX_GUIWINDOW_DEC(EntityHierarchy);
private:
	static void UpdateScene(std::shared_ptr<BaseScene> pScene, int id);
	static void UpdateEntity(std::shared_ptr<BaseScene> pScene, entt::entity handle);
};