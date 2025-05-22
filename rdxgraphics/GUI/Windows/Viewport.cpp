#include <pch.h>
#include "Viewport.h"
#include "ECS/Systems/RenderSystem.h"
#include "GSM/SceneManager.h"

void Viewport::UpdateImpl(float dt)
{
	if (!ImGui::BeginTable("##viewport_table", 1))
		return;

	ImGuiTableColumnFlags colFlags = ImGuiTableColumnFlags_WidthStretch;
	ImGui::TableSetupColumn("viewport_col0", colFlags);

	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	if (ImGui::Button("Restart Scene"))
	{
		SceneManager::Restart();
	}

	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	ImVec2 currPos = ImGui::GetCursorPos();
	auto& pass = RenderSystem::GetScreenPass();
	ImVec2 winDims{
		ImGui::GetWindowContentRegionMax().x - currPos.x,
		ImGui::GetWindowContentRegionMax().y - currPos.y
	};
	glm::vec2 bufDims = pass.GetBufferDims();
	float ar = bufDims.x / bufDims.y;
	glm::vec2 vpSize{};

	{ // restrict size based on imgui window height. 
		// larger ar == wider window == base on height
		if ((winDims.x / winDims.y) > ar)
		{
			vpSize = {
				winDims.y * ar,
				winDims.y
			};
		}
		// smaller ar == shorter window == base on wdth
		else
		{
			vpSize = {
				winDims.x,
				winDims.x / ar
			};
		}
	}

	GLuint const frameHandle = pass.GetTextureBuffer();
	ImGui::Image(frameHandle, 
		ImVec2{ vpSize.x, vpSize.y },
		{ 0.f, 1.f }, { 1.f, 0.f }); // It's flipped vertically

	ImGui::EndTable();
}