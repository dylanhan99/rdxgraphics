#include "Camera.h"
#include "Utils/Input.h"
#include "GLFWWindow/GLFWWindow.h"
#include "ECS/EntityManager.h"
#include "BoundingVolume.h"

Camera::Camera(
	Mode camMode,
	glm::vec2 aspect, float fov, glm::vec2 clipPlanes)
{
	m_AspectRatio = aspect.s / aspect.t;
	m_FOV = fov;
	m_CameraMode = camMode;
	m_Clip = clipPlanes;

	EventDispatcher<int, int>::RegisterEvent(RX_EVENT_FRAMEBUFFER_RESIZE,
		[&](int x, int y)
		{
			m_AspectRatio = (float)x / (float)y;
		});
}

void Camera::OnConstructImpl()
{
	entt::entity const handle = GetEntityHandle();
	if (!EntityManager::HasComponent<Xform>(handle))
	{ // Add an xform if not found
		EntityManager::AddComponent<Xform>(handle, glm::vec3{}, glm::vec3{ 1.f }, DefaultFront);
	}

	// Overrides the existing one if need be
	EntityManager::AddComponent<BoundingVolume>(handle, BV::Frustum);

	UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
	// [todo] add a camera::dirty, UpdateCameraVectors should only run if either xform or camera dirty

	Xform& xform = EntityManager::GetComponent<Xform>(GetEntityHandle());
	glm::vec3 const& position = xform.GetTranslate();
	glm::vec3& eulerOrientation = xform.GetEulerOrientation();

	glm::vec3 m_Up = g_WorldUp;
	glm::vec3 m_Right{};

	m_Front = glm::normalize(glm::quat{ eulerOrientation } * DefaultFront);
	m_Right = glm::normalize(glm::cross(m_Front, g_WorldUp)); // Using world-up here, assuming we are NOT allowing cam to roll
	m_Up	= glm::normalize(glm::cross(m_Right, m_Front));

	m_ViewMatrix =
		glm::lookAt(position, position + m_Front, m_Up);

	if (m_CameraMode == Mode::Perspective)
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

void Camera::Inputs(float dt)
{
	Xform& xform = EntityManager::GetComponent<Xform>(GetEntityHandle());
	glm::vec3& position = xform.GetTranslate();
	glm::vec3& eulerOrientation = xform.GetEulerOrientation();

	// Speed modifiers
	float moveSpeed = m_MovementSpeed * dt;
	if (Input::IsKeyDown(RX_KEY_LEFT_CONTROL))
		moveSpeed *= 2.f;
	
	if (Input::IsKeyDown(RX_KEY_W))
		position += moveSpeed * glm::vec3{ m_Front.x, 0.f, m_Front.z };
	if (Input::IsKeyDown(RX_KEY_S))
		position -= moveSpeed * glm::vec3{ m_Front.x, 0.f, m_Front.z };
	if (Input::IsKeyDown(RX_KEY_D))
		position += moveSpeed * glm::normalize(glm::cross(m_Front, g_WorldUp));
	if (Input::IsKeyDown(RX_KEY_A))
		position -= moveSpeed * glm::normalize(glm::cross(m_Front, g_WorldUp));
	
	if (Input::IsKeyDown(RX_KEY_SPACE))
		position += moveSpeed * g_WorldUp;
	if (Input::IsKeyDown(RX_KEY_LEFT_SHIFT))
		position += moveSpeed * -g_WorldUp;
	
	if (Input::IsMouseScrolled())
	{
		if (m_CameraMode == Mode::Perspective)
			position += ((float)Input::GetMouseScrollOffset() * m_ZoomSpeed) * m_Front;
		else
			m_OrthoSize += (float)Input::GetMouseScrollNormalized() * m_ZoomSpeed;
	}

	glm::vec2 cursorPos  = (glm::vec2)GLFWWindow::GetCursorPos();
	glm::vec2 windowDims = (glm::vec2)GLFWWindow::GetWindowDims();
	
	float pitch = windowDims.y ? m_PitchSpeed * ((windowDims.y * 0.5f - cursorPos.y) / windowDims.y) : 0.f;
	float yaw	= windowDims.x ? m_YawSpeed	  * ((windowDims.x * 0.5f - cursorPos.x) / windowDims.x) : 0.f;
	
	eulerOrientation.x = glm::clamp(eulerOrientation.x + pitch, 
		-glm::half_pi<float>() + glm::radians(1.f),
		 glm::half_pi<float>() - glm::radians(1.f));
	eulerOrientation.y += yaw;

	if (EntityManager::HasComponent<BoundingVolume>(GetEntityHandle()))
		EntityManager::GetComponent<BoundingVolume>(GetEntityHandle()).SetDirty();
	GLFWWindow::CenterCursor();
}

glm::vec3 Camera::GetPosition() const
{
	RX_ASSERT(EntityManager::HasEntity(GetEntityHandle()));

	Xform& xform = EntityManager::GetComponent<Xform>(GetEntityHandle());
	return xform.GetTranslate();
}

glm::vec3 Camera::GetEulerOrientation() const
{
	RX_ASSERT(EntityManager::HasEntity(GetEntityHandle()));

	Xform& xform = EntityManager::GetComponent<Xform>(GetEntityHandle());
	return xform.GetEulerOrientation();
}

glm::vec3 Camera::GetDirection() const
{
	RX_ASSERT(EntityManager::HasEntity(GetEntityHandle()));

	Xform& xform = EntityManager::GetComponent<Xform>(GetEntityHandle());
	return glm::quat{ xform.GetEulerOrientation() } * glm::vec3{ 1.f,0.f,0.f };
}

