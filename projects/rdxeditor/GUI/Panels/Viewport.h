#ifndef VIEWPORT_H
#define VIEWPORT_H
#include "GUI/BasePanel.h"

namespace rdxgui
{
	class ToolBar final : public BasePanel
	{
	public:
		ToolBar(const char* windowName, ImGuiWindowFlags flags) : BasePanel(windowName, flags) {}

	private:
		void UpdateImpl(float) override;
	};

	class EngineViewport final : public BasePanel
	{
	public:
		EngineViewport(const char* windowName, ImGuiWindowFlags flags) : BasePanel(windowName, flags) {}

	private:
		void UpdateImpl(float) override;
	};

	class GameViewport final : public BasePanel
	{
	public:
		GameViewport(const char* windowName, ImGuiWindowFlags flags) : BasePanel(windowName, flags) {}

	private:
		void UpdateImpl(float) override;
	};
}

#endif