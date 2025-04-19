#include <pch.h>
#include "Camera.h"

#include "Utils/Input.h"
#include "GLFWWindow/GLFWWindow.h"

Camera::Camera(
	glm::vec3 const& position, 
	glm::vec3 const& orientation, 
	glm::vec2 aspect, float fov, Mode camMode)
	: m_Position(position), m_EulerOrientation(orientation),
	m_AspectRatio(aspect.s / aspect.t), m_FOV(fov), m_CameraMode(camMode)
{
	UpdateCameraVectors();

	EventDispatcher<double, double>::RegisterEvent(RX_EVENT_SCROLL_CALLBACK,
		[&](double, double yoffset)
		{
			if (!IsCameraInUserControl())
				return;

			float moveSpeed = m_ZoomSpeed;
			if (Input::IsKeyDown(GLFW_KEY_LEFT_CONTROL))
				moveSpeed *= 2.f;

			m_Position += ((float)yoffset * moveSpeed) * glm::vec3{ m_Front.x, 0.f, m_Front.z };
		});
}

void Camera::UpdateCameraVectors()
{
	m_Front = glm::vec3{
		glm::cos(m_EulerOrientation.y) * glm::cos(m_EulerOrientation.x),
		glm::sin(m_EulerOrientation.x),
		glm::sin(m_EulerOrientation.y) * glm::cos(m_EulerOrientation.x)
	};

	glm::vec3 m_Up = m_WorldUp;
	glm::vec3 m_Right{};

	m_Front = glm::normalize(m_Front);
	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp)); // Using world-up here, assuming we are NOT allowing cam to roll
	m_Up	= glm::normalize(glm::cross(m_Right, m_Front));

	m_ViewMatrix =
		glm::lookAt(m_Position, m_Position + m_Front, m_Up);

	if (m_CameraMode == Mode::Perspective)
	{
		m_ProjectionMatrix =
			glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_Clip.s, m_Clip.t);
	}
	else
	{
		// This represents "zoom". But since this is a 3d camera, we can use Z axis instead.
		//float m_OrthoSize = 1.f; 
		float m_OrthoSize = m_Position.z;
		float halfWidth = m_OrthoSize * m_AspectRatio * 0.5f;
		float halfHeight = m_OrthoSize * 0.5f;

		m_ProjectionMatrix = glm::ortho(
			-halfWidth, halfWidth,		// left, right
			-halfHeight, halfHeight,	// bottom, top
			m_Clip.s, m_Clip.t			// near, far
		);
	}
}

void Camera::Inputs(float dt)
{
	// Speed modifiers
	float moveSpeed = m_MovementSpeed * dt;
	if (Input::IsKeyDown(GLFW_KEY_LEFT_CONTROL))
		moveSpeed *= 2.f;

	if (Input::IsKeyDown(GLFW_KEY_W))
		m_Position += moveSpeed * glm::vec3{ m_Front.x, 0.f, m_Front.z };
	if (Input::IsKeyDown(GLFW_KEY_S))
		m_Position -= moveSpeed * glm::vec3{ m_Front.x, 0.f, m_Front.z };
	if (Input::IsKeyDown(GLFW_KEY_D))
		m_Position += moveSpeed * glm::normalize(glm::cross(m_Front, m_WorldUp));
	if (Input::IsKeyDown(GLFW_KEY_A))
		m_Position -= moveSpeed * glm::normalize(glm::cross(m_Front, m_WorldUp));

	if (Input::IsKeyDown(GLFW_KEY_SPACE))
		m_Position += moveSpeed * m_WorldUp;
	if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
		m_Position += moveSpeed * -m_WorldUp;

	// Scroll controls, see ctor. Registered to scroll event.

	glm::vec2 cursorPos  = (glm::vec2)GLFWWindow::GetCursorPos();
	glm::vec2 windowDims = (glm::vec2)GLFWWindow::GetWindowDims();

	float pitch = windowDims.y ? m_YawSpeed   * ((windowDims.y * 0.5f - cursorPos.y) / windowDims.y) : 0.f;
	float yaw	= windowDims.x ? m_PitchSpeed * ((cursorPos.x - windowDims.x * 0.5f) / windowDims.x) : 0.f;

	m_EulerOrientation.x += pitch;
	m_EulerOrientation.y += yaw;

	GLFWWindow::CenterCursor();
}
