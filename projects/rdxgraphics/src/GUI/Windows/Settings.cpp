#include "Settings.h"
#include "GUI/GUI.h"
#include "GSM/SceneManager.h"
#include "GLFWWindow/GLFWWindow.h"
#include "ECS/Systems/RenderSystem.h"

void Settings::UpdateImpl(float dt)
{
	ImGuiTreeNodeFlags scnFlags =
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_SpanFullWidth;

	if (ImGui::TreeNodeEx("Bounding Volumes", scnFlags | ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Recalculate ALL BVs"))
		{
			for (auto [handle, boundingVolume] : EntityManager::View<BoundingVolume>().each())
				boundingVolume.SetDirty();
		}

		{
			int* pBV = reinterpret_cast<int*>(&BVHSystem::GetGlobalBVType());
			ImGui::SeparatorText("CurrentBV Option");
			BV prevGlobalBV = BVHSystem::GetGlobalBVType();
			bool isRadiod = false;
			isRadiod |= ImGui::RadioButton("AABB", pBV, static_cast<int>(BV::AABB)); ImGui::SameLine();
			isRadiod |= ImGui::RadioButton("OBB", pBV, static_cast<int>(BV::OBB)); ImGui::SameLine();
			isRadiod |= ImGui::RadioButton("Sphere", pBV, static_cast<int>(BV::Sphere));

			if (isRadiod && ((BV)*pBV != prevGlobalBV))
				BVHSystem::EnforceUniformBVs();
		}

		{
			int* algorithm = reinterpret_cast<int*>(&SphereBV::Algorithm);
			ImGui::SeparatorText("SphereBV Options");
			bool isRadiod = false;
			isRadiod |= ImGui::RadioButton("Ritter", algorithm, static_cast<int>(SphereBV::Algo::Ritter)); ImGui::SameLine();
			isRadiod |= ImGui::RadioButton("Larsson", algorithm, static_cast<int>(SphereBV::Algo::Larsson)); ImGui::SameLine();
			isRadiod |= ImGui::RadioButton("PCA", algorithm, static_cast<int>(SphereBV::Algo::PCA));

			if (isRadiod && BVHSystem::GetGlobalBVType() == BV::Sphere) // Set ALL spheres to be dirty, to be updated with new algorithm
			{
				auto view = EntityManager::View<const BoundingVolume, SphereBV>();
				for (auto [handle, boundingVolume, bv] : view.each())
					bv.SetDirtyBV();
			}
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("GSM", scnFlags | ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Restart Scene"))
			SceneManager::Restart();

		auto const& scenes = SceneManager::GetScenes();
		if (ImGui::BeginCombo("Select Scene", scenes[SceneManager::GetCurrScene()]->GetSceneName().c_str()))
		{
			for (size_t i = 0; i < scenes.size(); ++i)
			{
				auto const& pScene = scenes[i];
				if (!pScene)
					continue;

				bool selected = i == SceneManager::GetCurrScene();
				if (ImGui::Selectable(pScene->GetSceneName().c_str(), &selected))
				{
					SceneManager::SetNextScene(i);
				}
			}
			ImGui::EndCombo();
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Graphics", scnFlags | ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SeparatorText("Toggle Passes");
		for (auto& pass : RenderSystem::GetRenderPasses())
		{
			ImGui::Checkbox(pass->GetDisplayName().c_str(), &pass->IsEnabled());
		}

		ImGui::SeparatorText("Global Lighting");
		ImGui::ColorEdit3("Ambience", glm::value_ptr(RenderSystem::GetGlobalIllumination()));
		ImGui::DragFloat("Ambiance Factor", &RenderSystem::GetGlobalIllumination().w, 0.05f, 0.f, 1.f, "%.2f");

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Window", scnFlags))
	{
		static int s_FPSIndex = 0; // Defaults at 30
		static std::array<const char*, 6> s_FPSs{ "30", "60", "120", "144", "240", "No Lim." };
		static std::array<uint32_t, 6> s_FPSsNum{ 30, 60, 120, 144, 240, std::numeric_limits<uint32_t>::infinity() };
		if (ImGui::SliderInt("FPS", &s_FPSIndex, 0, static_cast<int>(s_FPSs.size() - 1), s_FPSs[s_FPSIndex]))
			GLFWWindow::SetTargetFPS(s_FPSsNum[s_FPSIndex]);

		ImGui::BeginDisabled();
		if (ImGui::Checkbox("VSync", &GLFWWindow::IsVSync()))
			GLFWWindow::SetIsVSync(GLFWWindow::IsVSync());
		ImGui::EndDisabled();

		ImGui::TreePop();
	}
}
