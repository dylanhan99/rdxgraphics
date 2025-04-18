#pragma once

class Camera
{
public:
	Camera(
		glm::vec3 const& position = { 0.f, 0.f, 0.f },
		glm::vec3 const& orientation = { 0.f,0.f,-1.f },
		glm::vec2 aspect = { 4.f, 3.f },
		float fov = 90
	);

	void UpdateCameraVectors();
	void Inputs();

	inline glm::mat4 const& GetViewMatrix() const { return m_ViewMatrix; }
	inline glm::mat4 const& GetProjMatrix() const { return m_ProjectionMatrix; }

private:
	inline static glm::vec3 m_WorldUp{ 0.f,1.f,0.f };

	glm::mat4 m_ViewMatrix{ glm::mat4(1.f) };
	glm::mat4 m_ProjectionMatrix{ glm::mat4(1.f) };
	glm::vec3 m_Position{}; // World
	glm::vec3 m_Orientation{};
	glm::vec3 m_Front{};
	float m_AspectRatio{};
	float m_FOV{};

	glm::vec2 m_Clip{ 0.f, 100.f };
	float m_MovementSpeed{ 1.f };
};