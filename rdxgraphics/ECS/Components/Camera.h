#pragma once
#include "BaseComponent.h"

class Camera : public BaseComponent
{
RX_COMPONENT_HAS_HANDLE(Camera);
public:
	inline static const glm::vec3 DefaultFront{ 0.f,0.f,-1.f };
	enum class Mode {
		Perspective,
		Orthorgonal
	};

public:
	Camera(
		Mode camMode,
		glm::vec2 aspect = { 4.f, 3.f },
		float fov = 90
	);

	void OnConstructImpl() override;

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

	glm::vec3 GetPosition() const;
	glm::vec3 GetEulerOrientation() const;
	glm::vec3 GetDirection() const;

	inline float const& GetOrthoSize() const { return m_OrthoSize; }
	inline float& GetOrthoSize() { return m_OrthoSize; }

	inline bool IsPerspective() const { return m_CameraMode == Mode::Perspective; }
	inline bool IsOrtho() const { return m_CameraMode == Mode::Orthorgonal; }

private:
	glm::mat4 m_ViewMatrix{ glm::mat4(1.f) };
	glm::mat4 m_ProjectionMatrix{ glm::mat4(1.f) };

	glm::vec2 m_Clip{ 0.1f, 100.f };
	glm::vec3 m_Front{};
	float m_AspectRatio{};
	float m_FOV{};
	float m_MovementSpeed{ 1.f };
	float m_PitchSpeed{ 1.f };
	float m_YawSpeed{ 1.f };
	float m_ZoomSpeed{ 0.1f }; // Scrolling scale factor. Meant for finer fwd/bwd movement

	float m_OrthoSize = 10.f;

	Mode m_CameraMode{};
	bool m_CameraInUserControl{ false };
};