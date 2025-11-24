#include "CameraComponent.h"

using namespace rdx;

CameraComponent::CameraComponent(
	Mode camMode,
	glm::vec2 aspect, float fov, glm::vec2 clipPlanes)
{
	m_AspectRatio = aspect.s / aspect.t;
	m_FOV = fov;
	m_CameraMode = camMode;
	m_Clip = clipPlanes;

	//EventDispatcher<int, int>::RegisterEvent(RX_EVENT_FRAMEBUFFER_RESIZE,
	//	[&](int x, int y)
	//	{
	//		m_AspectRatio = (float)x / (float)y;
	//	});
}

void CameraComponent::UpdateCameraVectors(glm::vec3 const& camPos, glm::vec3 const& eulerOrientation)
{
	glm::vec3 m_Up = WorldUp;

	m_Front = glm::normalize(glm::quat{ eulerOrientation } *DefaultFront);
	m_Right = glm::normalize(glm::cross(m_Front, m_Up)); // Using world-up here, assuming we are NOT allowing cam to roll
	//m_Up = glm::normalize(glm::cross(m_Right, m_Front));

	m_ViewMatrix = glm::lookAt(camPos, camPos + m_Front, m_Up);

	if (IsPerspective())
	{
		m_ProjectionMatrix =
			glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_Clip.s, m_Clip.t);
	}
	else
	{
		float halfWidth = m_OrthoSize * m_AspectRatio * 0.5f;
		float halfHeight = m_OrthoSize * 0.5f;

		m_ProjectionMatrix = glm::ortho(
			-halfWidth, halfWidth,		// left, right
			-halfHeight, halfHeight,	// bottom, top
			m_Clip.s, m_Clip.t			// near, far
		);
	}
}
