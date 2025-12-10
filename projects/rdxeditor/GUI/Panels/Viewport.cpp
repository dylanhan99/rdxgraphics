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

void EngineViewport::UpdateImpl(float dt)
{
	using namespace rdx;

	ImVec2 const size = ImGui::GetContentRegionAvail();
	ImGui::Image(editorPass->m_TextureBuffer, size);

	TransformComponent& camXform = ((RDXGui*)ServiceLayer::ApplicationService())->GetCameraPosition();
	CameraComponent& cam = ((RDXGui*)ServiceLayer::ApplicationService())->GetCamera();
	cam.SetAspectRatio(glm::vec2{ size.x, size.y });

	// Movement
	if (ImGui::IsWindowFocused())
	{
		Input const& input = *ServiceLayer::InputService();
		glm::vec2 const& mousePosDelta = input.GetMousePosDelta();
		glm::vec2 const mousePosDeltaDT = mousePosDelta * dt;
		glm::vec3 const& camFront = cam.GetFront();

		if (input.IsMouseDown(MouseCode::Middle))
		{ // Keyboard input
			constexpr float m_CamSpeed = 1.f;
			glm::vec3 const camRight = glm::normalize(glm::cross(camFront, cam.WorldUp));
			glm::vec3 const camUp = cam.WorldUp;

			glm::vec3& camPos = camXform.Position;

			glm::vec3 translation{};
			translation += mousePosDeltaDT.x * camRight * m_CamSpeed; // Horizontal translation
			translation += mousePosDeltaDT.y * camUp * m_CamSpeed; // Vertical translation

			camPos += translation;
		}

		else if (input.IsMouseDown(MouseCode::Right))
		{ // Mouse movement
			constexpr float m_PitchSpeed = 1.f;
			constexpr float m_YawSpeed = 1.f;
			const float pitch = mousePosDeltaDT.y * m_PitchSpeed; // x and y need to be inverted so the camera is FPS-like. Y already is inverted due to topleft of window being (0,0)
			const float yaw = -mousePosDeltaDT.x * m_YawSpeed;

			glm::vec3& eulerOrientation = camXform.Rotation;
				eulerOrientation.x = glm::clamp(eulerOrientation.x + pitch,
					-glm::half_pi<float>() + glm::radians(1.f),
					glm::half_pi<float>() - glm::radians(1.f));
			eulerOrientation.y += yaw;
		}

		constexpr float m_ZoomSpeed = 5.f;
		// Zoom in/out
		if (input.GetScrollDelta() > 0.f)
		{
			glm::vec3& camPos = camXform.Position;
			camPos += camFront * m_ZoomSpeed * dt;
		}
		else if (input.GetScrollDelta() < 0.f)
		{
			glm::vec3& camPos = camXform.Position;
			camPos -= camFront * m_ZoomSpeed * dt;
		}

	}
}

void GameViewport::UpdateImpl(float)
{
	ImGui::Image(gamePass->m_TextureBuffer, ImVec2{ 600, 600 });
}