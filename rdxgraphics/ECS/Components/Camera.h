#pragma once
#include "BaseComponent.h"

class Camera : public BaseComponent
{
RX_COMPONENT_HAS_HANDLE(Camera);
public:
	enum class Mode {
		Perspective,
		Orthorgonal
	};

public:
	Camera(
		entt::entity handle,
		glm::vec3 const& position,
		glm::vec3 const& orientation = { 0.f,0.f,-1.f },
		glm::vec2 aspect = { 4.f, 3.f },
		float fov = 90,
		Mode camMode = Mode::Perspective
	);

	void UpdateCameraVectors();
	void Inputs(float dt);

	inline glm::mat4 const& GetViewMatrix() const { return m_ViewMatrix; }
	inline glm::mat4 const& GetProjMatrix() const { return m_ProjectionMatrix; }
	//inline glm::vec3 const& GetEulerOrientation() const { return m_EulerOrientation; }
	//inline glm::vec3& GetEulerOrientation() { return m_EulerOrientation; }
	//inline glm::vec3 const& GetPosition() const { return m_Position; }
	//inline glm::vec3& GetPosition() { return m_Position; }

	inline float& GetFOV() { return m_FOV; }
	inline glm::vec2& GetClipPlanes() { return m_Clip; }
	inline float& GetMovementSpeed() { return m_MovementSpeed; }
	inline float& GetPitchSpeed() { return m_PitchSpeed; }
	inline float& GetYawSpeed() { return m_YawSpeed; }

	inline bool IsCameraInUserControl() const { return m_CameraInUserControl; }
	inline bool& IsCameraInUserControl() { return m_CameraInUserControl; }

private:
	glm::mat4 m_ViewMatrix{ glm::mat4(1.f) };
	glm::mat4 m_ProjectionMatrix{ glm::mat4(1.f) };

	glm::vec2 m_Clip{ 0.f, 100.f };
	glm::vec3 m_Front{};
	float m_AspectRatio{};
	float m_FOV{};
	float m_MovementSpeed{ 1.f };
	float m_PitchSpeed{ 1.f };
	float m_YawSpeed{ 1.f };
	float m_ZoomSpeed{ 0.1f }; // Scrolling scale factor. Meant for finer fwd/bwd movement

	Mode m_CameraMode{};
	bool m_CameraInUserControl{ false };
};