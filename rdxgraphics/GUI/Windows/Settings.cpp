#include <pch.h>
#include "Settings.h"
#include "GUI/GUI.h"
#include "GSM/SceneManager.h"
#include "GLFWWindow/GLFWWindow.h"
#include "Graphics/RenderPass.h"

extern bool hasDefault;
extern bool hasWireframe;
extern bool hasMinimap;
extern int renderOption;
extern RenderPass minimapPass;

void Settings::UpdateImpl(float dt)
{
	ImGui::SeparatorText("Graphics");
	ImGui::Checkbox("Base", &hasDefault);
	ImGui::Checkbox("Wireframe", &hasWireframe);
	ImGui::Checkbox("Minimap", &hasMinimap);
	//ImGui::ColorEdit3("Minimap Color", glm::value_ptr(minimapPass.GetBackbufferColor())); dosnt work as i expected

	static int s_FPSIndex = 0; // Defaults at 30
	static std::array<const char*, 6> s_FPSs{ "30", "60", "120", "144", "240", "No Lim." };
	static std::array<uint32_t, 6> s_FPSsNum{ 30, 60, 120, 144, 240, std::numeric_limits<uint32_t>::max() };
	ImGui::SeparatorText("Window");
	if (ImGui::SliderInt("FPS", &s_FPSIndex, 0, static_cast<int>(s_FPSs.size() - 1), s_FPSs[s_FPSIndex]))
		GLFWWindow::SetTargetFPS(s_FPSsNum[s_FPSIndex]);
	if (ImGui::Checkbox("VSync", &GLFWWindow::IsVSync()))
		GLFWWindow::SetIsVSync(GLFWWindow::IsVSync());
}
