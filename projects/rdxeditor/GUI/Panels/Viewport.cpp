#include "Viewport.h"

#include "rdxengine/Graphics/MultiPassing/Passes/TestPass.h"
std::shared_ptr<rdx::TestPass> PPP{};

void ToolBar::UpdateImpl(float)
{
	// Supposed to be the play/stop toolbar. Sits under the menubar
	ImGui::Text("Toolbar");
}

void EngineViewport::UpdateImpl(float)
{
	ImVec2 const size = ImGui::GetContentRegionAvail();
	ImGui::Image(PPP->m_TextureBuffer, size);
}

void GameViewport::UpdateImpl(float)
{
	ImGui::Image(PPP->m_TextureBuffer, ImVec2{ 600, 600 });
}