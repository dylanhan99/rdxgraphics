#include <pch.h>
#include "EntityHierarchy.h"
#include "GUI/GUI.h"
#include "GSM/SceneManager.h"

void EntityHierarchy::UpdateImpl(float dt)
{
	UpdateScene(SceneManager::GetCommonScene(), 0);
	UpdateScene(SceneManager::GetWorkingScene(), 1);
}

void EntityHierarchy::UpdateScene(std::shared_ptr<BaseScene> pScene, int id)
{
	if (!pScene)
		return;

	auto const& entities = pScene->GetEntities();

	ImGuiTreeNodeFlags scnFlags =
		ImGuiTreeNodeFlags_OpenOnArrow | 
		ImGuiTreeNodeFlags_SpanFullWidth;
	if (entities.empty())
		scnFlags |= ImGuiTreeNodeFlags_Leaf;
	else
		scnFlags |= ImGuiTreeNodeFlags_DefaultOpen;

	std::string name = pScene->GetSceneName() + "##" + std::to_string(id);
	bool sceneOpen = ImGui::TreeNodeEx(name.c_str(), scnFlags);
	std::string const& desc = pScene->GetSceneDesc();
	if (!desc.empty())
		ImGui::SetItemTooltip(desc.c_str());

	if (sceneOpen)
	{
		name = "Ent #";
		for (auto handle : entities)
		{
			ImGuiTreeNodeFlags flags = 
				ImGuiTreeNodeFlags_Leaf | 
				ImGuiTreeNodeFlags_SpanFullWidth;
			if (handle == GUI::GetSelectedEntity())
				flags |= ImGuiTreeNodeFlags_Selected;

			std::string id = name + std::to_string((uint32_t)handle);
			if (ImGui::TreeNodeEx(id.c_str(), flags))
				ImGui::TreePop();

			if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				GUI::SetSelectedEntity(handle);
		}

		ImGui::TreePop();
	}
}