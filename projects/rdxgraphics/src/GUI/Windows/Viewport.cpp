#include "Viewport.h"
#include "ECS/Systems/RenderSystem.h"
#include "GSM/SceneManager.h"
#include "Graphics/Passes/Passes.h"
#include "GUI/GUI.h"
#include "GLFWWindow/GLFWWindow.h"

void Viewport::UpdateImpl(float dt)
{
	if (!ImGui::BeginTable("##viewport_table", 1))
		return;

	ImGuiTableColumnFlags colFlags = ImGuiTableColumnFlags_WidthStretch;
	ImGui::TableSetupColumn("viewport_col0", colFlags);

	// Start viewport settings row
	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	ToolBar();

	// Start image row
	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	EngineView();

	ImGui::EndTable();
}

void Viewport::ToolBar()
{
	int* pOperation = reinterpret_cast<int*>(&GUI::GetGuizmoOperation());
	ImGui::Text("Guizmo");
	ImGui::SameLine();
	ImGui::RadioButton("Translate", pOperation, (int)ImGuizmo::OPERATION::TRANSLATE);
	if (ImGui::BeginItemTooltip())
	{
		ImGui::Text("Alt + Q");
		ImGui::EndTooltip();
	}
	ImGui::SameLine();
	ImGui::RadioButton("Scale", pOperation, (int)ImGuizmo::OPERATION::SCALE);
	if (ImGui::BeginItemTooltip())
	{
		ImGui::Text("Alt + W");
		ImGui::EndTooltip();
	}
	ImGui::SameLine();
	ImGui::RadioButton("Rotate", pOperation, (int)ImGuizmo::OPERATION::ROTATE);
	if (ImGui::BeginItemTooltip())
	{
		ImGui::Text("Alt + R");
		ImGui::EndTooltip();
	}
	ImGui::Separator();
}

void Viewport::EngineView()
{
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
	ImVec2 imagePosAbs = ImGui::GetCursorScreenPos();
	ImVec2 imageSize{ vpSize.x, vpSize.y };
	ImGui::Image(frameHandle,
		imageSize,
		{ 0.f, 1.f }, { 1.f, 0.f }); // It's flipped vertically

	if (!Guizmos(imagePosAbs, imageSize))
		Picking(imagePosAbs, imageSize, bufDims); // Only pick if not interacting with gizmos
}

void Viewport::Picking(ImVec2 const& imagePos, ImVec2 const& imageSize, glm::vec2 const& actualBufferSize)
{
	entt::entity const camHandle = RenderSystem::GetActiveCamera();
	if (!EntityManager::HasEntity(camHandle) || !EntityManager::HasComponent<Camera>(camHandle))
		return;

	ImVec2 imWinPos = ImGui::GetWindowPos();
	ImGui::SetCursorScreenPos(imagePos);
	ImGui::InvisibleButton("viewport_picking", imageSize);
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGuizmo::IsUsingAny())
	{
		// Get the % along width and height of where the mouse is and get the screen coords
		glm::vec2 mousePos{ ImGui::GetMousePos().x, ImGui::GetMousePos().y };
		glm::vec4 point = { // ImGui vp offset first
			mousePos.x - imagePos.x,
			imageSize.y - (mousePos.y - imagePos.y),
			0.f, 1.f };

		// Now ndc
		point.x = (point.x / imageSize.x) * 2.f - 1.f;
		point.y = (point.y / imageSize.y) * 2.f - 1.f;
		point.z = -1.f; // Near plane

		// Then inverse all the way back to world coords
		Camera const& cam = EntityManager::GetComponent<Camera const>(camHandle);
		glm::mat4 const invMatrix = glm::inverse(cam.GetProjMatrix() * cam.GetViewMatrix());
		point = invMatrix * point;
		point /= point.w; // perspective division

		glm::vec3 direction = glm::normalize(glm::vec3{ point } - cam.GetPosition());
		entt::entity const rayHandle = EntityManager::CreateEntity();
		EntityManager::AddComponent<Xform>(rayHandle, point);
		EntityManager::AddComponent<Collider>(rayHandle, Primitive::Ray);
		RayPrimitive& ray = EntityManager::GetComponent<RayPrimitive>(rayHandle);
		ray.SetDirection(direction);

		// For each BV, if is OUT, skip
		// Else do check
		std::map<float, BVHNode const> tEs{};
		std::function<void(std::unique_ptr<BVHNode>&)> pick;
		pick = 
			[&ray, &tEs, &pick](std::unique_ptr<BVHNode>& pNode)
			{
				if (!pNode)
					return;

				BoundingVolume& boundingVolume = EntityManager::GetComponent<BoundingVolume>(pNode->Handle);
				if (pNode->IsLeaf())
				{
#define _RX_X(Klass)																   \
					case BV::Klass:													   \
					{																   \
						Klass##BV& bv = EntityManager::GetComponent<Klass##BV>(pNode->Handle);\
						if (bv.GetBVState() == BVState::Out) return;				   \
						float tE{}; /* time of entry */								   \
						bool intersect = CollisionSystem::CheckCollision(ray, bv, &tE);\
						if (intersect)												   \
							tEs.emplace(tE, BVHNode{*pNode});						   \
						break;														   \
					}

					switch (boundingVolume.GetBVType())
					{
						RX_DO_ALL_BVH_ENUM_M(_RX_X);
					default: break;
					}
#undef _RX_X
				}
				else // Node
				{
#define _RX_X(Klass)																   \
					case BV::Klass:													   \
					{																   \
						Klass##BV& bv = EntityManager::GetComponent<Klass##BV>(pNode->Handle);\
						if (bv.GetBVState() == BVState::Out) return;				   \
						bool intersect = CollisionSystem::CheckCollision(ray, bv);	   \
						if (intersect)												   \
						{															   \
							pick(pNode->Left);										   \
							pick(pNode->Right);										   \
						}															   \
						break;														   \
					}

					switch (boundingVolume.GetBVType())
					{
						RX_DO_ALL_BVH_ENUM_M(_RX_X);
					default: break;
					}
#undef _RX_X
				}
			};

		pick(BVHSystem::GetRootNode()); // Recursively pick the leaf node of interest

		entt::entity selectedEntity = entt::null;
		// Then iterate over the objects in the leaf node to determine your target
		for (auto const& [tE, node] : tEs)
		{
			std::map<float, entt::entity const> finaltEs{};
			for (auto const& handle : node.Objects)
			{
				BoundingVolume& boundingVolume = EntityManager::GetComponent<BoundingVolume>(handle);
#define _RX_X(Klass)															   \
				case BV::Klass:													   \
				{																   \
					Klass##BV& bv = EntityManager::GetComponent<Klass##BV>(handle);\
					if (bv.GetBVState() == BVState::Out) continue;				   \
					float tE{}; /* time of entry */								   \
					bool intersect = CollisionSystem::CheckCollision(ray, bv, &tE);\
					if (intersect)												   \
						finaltEs.emplace(tE, handle);							   \
					break;														   \
				}

				switch (boundingVolume.GetBVType())
				{
					RX_DO_ALL_BVH_ENUM_M(_RX_X);
				default: break;
				}
#undef _RX_X
				auto it = finaltEs.begin();
				if (it != finaltEs.end())
				{
					selectedEntity = it->second;
					break;
				}
			}

			if (selectedEntity != entt::null)
				break;
		}
		GUI::SetSelectedEntity(selectedEntity);
		EntityManager::Destroy(rayHandle);
	}
}

bool Viewport::Guizmos(ImVec2 const& imagePos, ImVec2 const& imageSize)
{
	entt::entity const selectedEntity = GUI::GetSelectedEntity();
	Camera& cam = EntityManager::GetComponent<Camera>(RenderSystem::GetActiveCamera());
	if (EntityManager::HasEntity(selectedEntity) && selectedEntity != RenderSystem::GetActiveCamera())
	{ // draw guizmo
		//ImGuizmo::SetGizmoSizeClipSpace(0.15f);
		ImGuizmo::BeginFrame();
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(imagePos.x, imagePos.y, imageSize.x, imageSize.y);

		Xform& xform = EntityManager::GetComponent<Xform>(GUI::GetSelectedEntity());
		glm::mat4 trData = xform.GetXform();

		ImGuizmo::Manipulate(
			glm::value_ptr(cam.GetViewMatrix()),
			glm::value_ptr(cam.GetProjMatrix()),
			GUI::GetGuizmoOperation(), ImGuizmo::WORLD, glm::value_ptr(trData));

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 translation{}, rotation{}, scale{};
			ImGuizmo::DecomposeMatrixToComponents(
				glm::value_ptr(trData), 
				glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));
			rotation = glm::radians(rotation);

			xform.SetTranslate(translation);
			xform.SetEulerOrientation(rotation);
			xform.SetScale(scale);
		}
	}

	return ImGuizmo::IsOver();
}