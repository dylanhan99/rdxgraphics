#ifndef INSPECTOR_H
#define INSPECTOR_H
#include "GUI/BasePanel.h"

namespace rdxgui
{
	class Inspector final : public BasePanel
	{
	public:
		Inspector(const char* windowName, ImGuiWindowFlags flags) : BasePanel(windowName, flags) {}

	private:
		void UpdateImpl(float dt) override;
	};
}

#endif