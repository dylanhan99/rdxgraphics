#include "Viewport.h"
#include "rdxengine/ServiceLayer.h"
#include "rdxeditor/GUI/RDXGui.h"

#include "rdxengine/Graphics/MultiPassing/Passes/TestPass.h"
std::shared_ptr<rdx::TestPass> editorPass{};
std::shared_ptr<rdx::TestPass> gamePass{};

using namespace rdxgui;

void ToolBar::UpdateImpl(float)
{
	// Supposed to be the play/stop toolbar. Sits under the menubar
	ImGui::Text("Toolbar");
}

void EngineViewport::UpdateImpl(float)
{
	ImVec2 const size = ImGui::GetContentRegionAvail();
	ImGui::Image(editorPass->m_TextureBuffer, size);

	((RDXGui*)rdx::ServiceLayer::ApplicationService())->GetCamera().SetAspectRatio(glm::vec2{ size.x, size.y });
}

void GameViewport::UpdateImpl(float)
{
	ImGui::Image(gamePass->m_TextureBuffer, ImVec2{ 600, 600 });
}