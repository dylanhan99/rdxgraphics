#include <pch.h>
#include "EntityHierarchy.h"
#include "GUI/GUI.h"

void EntityHierarchy::UpdateImpl(float dt)
{
	auto view = EntityManager::View<entt::entity>();

	std::string name = "Ent #";
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf;
	for (auto [handle] : view.each())
	{
		std::string id = name + std::to_string((uint32_t)handle);
		if (ImGui::TreeNodeEx(id.c_str(), flags))
			ImGui::TreePop();

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			GUI::SetSelectedEntity(handle);
	}
}
