#pragma once
#include "BaseGUIWindow.h"

class GUI : public BaseSingleton<GUI>
{
	RX_SINGLETON_DECLARATION(GUI);
public:
	static bool Init();
	static void Terminate();
	static void Update(double dt);
	static void Draw();

private:
	std::vector<std::unique_ptr<BaseGUIWindow>> m_GUIWindows{};
};