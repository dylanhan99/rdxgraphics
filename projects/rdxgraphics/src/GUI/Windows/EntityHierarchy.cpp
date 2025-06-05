#include <pch.h>
#include "EntityHierarchy.h"
#include "GUI/GUI.h"
#include "GSM/SceneManager.h"
#include "ECS/Components.h"

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
		for (auto handle : entities)
		{
			UpdateEntity(pScene, handle);
		}

		ImGui::TreePop();
	}
	if (ImGui::ButtonEx(("Create Entity##" + name).c_str(), {ImGui::GetWindowContentRegionMax().x , ImGui::GetTextLineHeight() * 2.f}))
	{
		static int count = 0;
		entt::entity handle = pScene->CreateDefaultEntity();
		EntityManager::GetComponent<Metadata>(handle).GetName() = "Entity (" + std::to_string(count++) + ")";

		GUI::SetSelectedEntity(handle);
	}

	ImGui::NewLine();
}

void EntityHierarchy::UpdateEntity(std::shared_ptr<BaseScene> pScene, entt::entity handle)
{
	ImGuiTreeNodeFlags flags =
		ImGuiTreeNodeFlags_Leaf |
		ImGuiTreeNodeFlags_SpanFullWidth;
	if (handle == GUI::GetSelectedEntity())
		flags |= ImGuiTreeNodeFlags_Selected;

	std::string id{};
	bool hasMeta = EntityManager::HasComponent<Metadata>(handle);
	if (hasMeta)
		id = EntityManager::GetComponent<Metadata>(handle).GetName();
	else
		id = "<META MISSING>";

	id += "##" + std::to_string((uint32_t)handle);
	bool isOpen = ImGui::TreeNodeEx(id.c_str(), flags);
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		GUI::SetSelectedEntity(handle);
	ImGui::SameLine();
	ImGui::BeginDisabled(EntityManager::HasComponent<NoDelete>(handle));
	if (ImGui::Button("X"))
	{
		pScene->DestroyEntity(handle);
	}
	ImGui::EndDisabled();

	if (isOpen)
	{
		ImGui::TreePop();
	}
}
