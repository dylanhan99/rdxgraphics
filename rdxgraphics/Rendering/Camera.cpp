#include <pch.h>
#include "Camera.h"

#include "Utils/Input.h"

Camera::Camera(
	glm::vec3 const& position, 
	glm::vec3 const& orientation, 
	glm::vec2 aspect, float fov)
	: m_Position(position), m_Orientation(orientation), 
	m_AspectRatio(aspect.s / aspect.t), m_FOV(fov)
{
	UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
	//m_Front = glm::vec3{
	//	glm::cos(GetEulerYaw()) * glm::cos(GetEulerPitch()),
	//	glm::sin(GetEulerPitch()),
	//	glm::sin(GetEulerYaw()) * glm::cos(GetEulerPitch())
	//};

	m_Front = m_Orientation;
	glm::vec3 m_Up = m_WorldUp;
	glm::vec3 m_Right{};

	m_Front = glm::normalize(m_Front);
	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp)); // Using worldright here, assuming we are NOT allowing cam to roll
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));

	m_ViewMatrix =
		glm::lookAt(m_Position, m_Position + m_Front, m_Up);

	m_ProjectionMatrix =
		glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_Clip.s, m_Clip.t);
}

void Camera::Inputs()
{
	// Speed modifiers
	if (Input::IsKeyDown(GLFW_KEY_LEFT_CONTROL))
		m_MovementSpeed = 0.4f;
	else if (Input::IsKeyUp(GLFW_KEY_LEFT_CONTROL))
		m_MovementSpeed = 0.1f;

	if (Input::IsKeyDown(GLFW_KEY_W))
		m_Position += m_MovementSpeed * glm::vec3{ m_Front.x, 0.f, m_Front.z };
	if (Input::IsKeyDown(GLFW_KEY_S))
		m_Position -= m_MovementSpeed * glm::vec3{ m_Front.x, 0.f, m_Front.z };
	if (Input::IsKeyDown(GLFW_KEY_D))
		m_Position += m_MovementSpeed * glm::normalize(glm::cross(m_Front, m_WorldUp));
	if (Input::IsKeyDown(GLFW_KEY_A))
		m_Position -= m_MovementSpeed * glm::normalize(glm::cross(m_Front, m_WorldUp));

	if (Input::IsKeyDown(GLFW_KEY_SPACE))
		m_Position += m_MovementSpeed * m_WorldUp;
	if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
		m_Position += m_MovementSpeed * -m_WorldUp;

	UpdateCameraVectors();
}
