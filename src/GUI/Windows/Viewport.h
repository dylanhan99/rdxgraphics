#pragma once
#include "GUI/BaseGUIWindow.h"

class Viewport : public BaseGUIWindow
{
	RX_GUIWINDOW_DEC(Viewport);

private:
	void ToolBar();
	void EngineView();
	void Picking(ImVec2 const& imagePos, ImVec2 const& imageSize, glm::vec2 const& actualBufferSize);
	bool Guizmos(ImVec2 const& imagePos, ImVec2 const& imageSize);
};