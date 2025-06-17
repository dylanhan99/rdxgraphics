#include "Viewport.h"
#include "ECS/Systems/RenderSystem.h"
#include "GSM/SceneManager.h"
#include "Graphics/Passes/Passes.h"
#include "GUI/GUI.h"

void Viewport::UpdateImpl(float dt)
{
	if (!ImGui::BeginTable("##viewport_table", 1))
		return;

	ImGuiTableColumnFlags colFlags = ImGuiTableColumnFlags_WidthStretch;
	ImGui::TableSetupColumn("viewport_col0", colFlags);

	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	ImVec2 currPos = ImGui::GetCursorPos();
	std::shared_ptr<BasePass> pass = RenderSystem::GetScreenPass();
	ImVec2 winDims{
		ImGui::GetWindowContentRegionMax().x - currPos.x,
		ImGui::GetWindowContentRegionMax().y - currPos.y
	};
	glm::vec2 bufDims = pass->GetBufferDims();
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

	GLuint const frameHandle = pass->GetTextureBuffer();
	ImVec2 imagePos = ImGui::GetCursorScreenPos();
	ImGui::Image(frameHandle, 
		ImVec2{ vpSize.x, vpSize.y },
		{ 0.f, 1.f }, { 1.f, 0.f }); // It's flipped vertically

	entt::entity const selectedEntity = GUI::GetSelectedEntity();
	Camera& cam = EntityManager::GetComponent<Camera>(RenderSystem::GetActiveCamera());
	if (EntityManager::HasEntity(selectedEntity) && selectedEntity != RenderSystem::GetActiveCamera())
	{ // draw guizmo
		//ImGuizmo::SetGizmoSizeClipSpace(0.15f);
		ImGuizmo::BeginFrame();
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(imagePos.x, imagePos.y, vpSize.x, vpSize.y);

		Xform& xform = EntityManager::GetComponent<Xform>(GUI::GetSelectedEntity());
		glm::mat4 trData = xform.GetXform();

		ImGuizmo::Manipulate(
			glm::value_ptr(cam.GetViewMatrix()), 
			glm::value_ptr(cam.GetProjMatrix()), 
			GUI::GetGuizmoOperation(), ImGuizmo::WORLD, glm::value_ptr(trData));

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 translate{}, scale{};
			glm::quat rotation{};
			glm::vec3 skew{};
			glm::vec4 perspective{};
			glm::decompose(trData, scale, rotation, translate, skew, perspective);

			xform.SetTranslate(translate);
			xform.SetScale(scale);
			xform.SetEulerOrientation(glm::eulerAngles(rotation));
		}
	}

	ImGui::EndTable();
}