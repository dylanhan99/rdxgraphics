#include <pch.h>
#include "Settings.h"
#include "GUI/GUI.h"
#include "GSM/SceneManager.h"
#include "GLFWWindow/GLFWWindow.h"
#include "Graphics/RenderPass.h"
#include "ECS/Systems/RenderSystem.h"

extern bool hasDefault;
extern bool hasWireframe;
extern bool hasMinimap;
extern int renderOption;

void Settings::UpdateImpl(float dt)
{
	ImGui::SeparatorText("GSM");
	{

		if (ImGui::Button("Restart Scene"))
		{
			SceneManager::Restart();
		}

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
	}

	ImGui::SeparatorText("Graphics");
	{
		ImGui::Checkbox("Base", &hasDefault);
		ImGui::Checkbox("Wireframe", &hasWireframe);
		ImGui::Checkbox("Minimap", &hasMinimap);
		ImGui::ColorEdit3("Global Ambience", glm::value_ptr(RenderSystem::GetGlobalIllumination()));
		ImGui::DragFloat("Ambiance Factor", &RenderSystem::GetGlobalIllumination().w, 0.05f, 0.f, 1.f, "%.2f");
	}

	ImGui::SeparatorText("Window");
	{
		static int s_FPSIndex = 0; // Defaults at 30
		static std::array<const char*, 6> s_FPSs{ "30", "60", "120", "144", "240", "No Lim." };
		static std::array<uint32_t, 6> s_FPSsNum{ 30, 60, 120, 144, 240, std::numeric_limits<uint32_t>::max() };
		if (ImGui::SliderInt("FPS", &s_FPSIndex, 0, static_cast<int>(s_FPSs.size() - 1), s_FPSs[s_FPSIndex]))
			GLFWWindow::SetTargetFPS(s_FPSsNum[s_FPSIndex]);
		if (ImGui::Checkbox("VSync", &GLFWWindow::IsVSync()))
			GLFWWindow::SetIsVSync(GLFWWindow::IsVSync());
	}
}
