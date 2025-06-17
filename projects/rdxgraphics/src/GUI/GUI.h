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

	inline static ImGuizmo::OPERATION GetGuizmoOperation() { return g.m_GuizmoOperation; }
	inline static void SetGuizmoOperation(ImGuizmo::OPERATION o) { g.m_GuizmoOperation = o; }

private:
	std::vector<std::unique_ptr<BaseGUIWindow>> m_GUIWindows{};
	entt::entity m_SelectedEntity{};
	ImGuiID m_DockID{};

	ImGuizmo::OPERATION m_GuizmoOperation{ ImGuizmo::OPERATION::TRANSLATE };
};