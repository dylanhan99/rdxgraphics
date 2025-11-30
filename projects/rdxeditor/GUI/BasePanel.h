#ifndef BASEPANEL_H
#define BASEPANEL_H
#include "rdxengine/BaseSystem.h"
#include <imgui.h>

class BasePanel : public rdx::BaseSystem
{
public:
	const char* GetWindowName() const { return m_WindowName; }
	
private:
	inline bool InitImpl() override { return true; }
	inline bool TerminateImpl() override { return true; }

	inline void UpdateImpl(float dt) override final
	{
		if (ImGui::Begin(m_WindowName, nullptr, m_Flags))
			UpdatePanel(dt);
		ImGui::End();
	}
	
	virtual void UpdatePanel(float dt) = 0;

protected:
	BasePanel(const char* windowName, ImGuiWindowFlags flags) : m_WindowName(windowName), m_Flags(flags) {}

private:
	const char* m_WindowName{};
	ImGuiWindowFlags m_Flags{};
};

#endif