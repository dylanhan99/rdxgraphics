#ifndef BASEPANEL_H
#define BASEPANEL_H
//#include "rdxengine/BaseSystem.h"
#include <imgui.h>

class BasePanel// : public rdx::BaseSystem
{
public:
	const char* GetWindowName() const { return m_WindowName; }
	virtual ~BasePanel() = default;

	inline bool Init()
	{
		return InitImpl();
	}
	
	inline bool Terminate()
	{
		return TerminateImpl();
	}

	inline void Update(float dt)
	{
		if (ImGui::Begin(m_WindowName, nullptr, m_Flags))
			UpdateImpl(dt);
		ImGui::End();
	}
	
private:
	inline virtual bool InitImpl() { return true; }
	inline virtual bool TerminateImpl() { return true; }
	inline virtual void UpdateImpl(float dt) = 0;

protected:
	BasePanel(const char* windowName, ImGuiWindowFlags flags) : m_WindowName(windowName), m_Flags(flags) {}

private:
	const char* m_WindowName{};
	ImGuiWindowFlags m_Flags{};
};

#endif