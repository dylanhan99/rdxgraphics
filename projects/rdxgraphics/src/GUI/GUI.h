#pragma once
#include "BaseGUIWindow.h"

class GUI : public BaseSingleton<GUI>
{
	RX_SINGLETON_DECLARATION(GUI);
public:
	static bool Init();
	static void Terminate();
	static void Update(float dt);
	static void Draw();

	inline static entt::entity GetSelectedEntity() { return g.m_SelectedEntity; }
	inline static void SetSelectedEntity(entt::entity handle) { g.m_SelectedEntity = handle; }

private:
	std::vector<std::unique_ptr<BaseGUIWindow>> m_GUIWindows{};
	entt::entity m_SelectedEntity{};
	ImGuiID m_DockID{};
};