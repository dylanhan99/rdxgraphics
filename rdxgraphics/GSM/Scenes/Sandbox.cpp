#include <pch.h>
#include "Sandbox.h"
#include "Utils/Input.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/EntityManager.h"
#include "ECS/Components/Camera.h"

void Sandbox::Load()
{
}

void Sandbox::Start()
{
	EntityManager::Destroy(GetEntities());

	{
		auto handle = BaseScene::CreateEntity();
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 1.f, 1.f, 1.f }, glm::vec3{ 1.f }, glm::vec3{ glm::quarter_pi<float>() });
		EntityManager::AddComponent<Model>(handle, Shape::Sphere);
		EntityManager::AddComponent<Collider>(handle, BV::Ray);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f,1.f,0.f });
	}
	{
		auto handle = BaseScene::CreateEntity();
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 1.f }, glm::vec3{ glm::quarter_pi<float>() });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Collider>(handle, BV::Triangle);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f,0.f,1.f });
	}
	{
		auto handle = BaseScene::CreateEntity();
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f, 5.f, 0.f }, glm::vec3{ 0.3f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<DirectionalLight>(handle);
	}
}

void Sandbox::Update(float dt)
{
	static float angle = 0.f;
	auto view = EntityManager::View<Xform, DirectionalLight>();
	for (auto [handle, xform, light] : view.each())
	{
		float rate = 0.25f;
		float radius = 5.f;
		angle += glm::two_pi<float>() * rate * dt;
		if (angle > glm::two_pi<float>()) angle = 0.f;

		xform.GetTranslate().x = glm::cos(angle) * radius;
		xform.GetTranslate().y = 0.f;
		xform.GetTranslate().z = glm::sin(angle) * radius;

		light.GetDirection() = glm::normalize(-xform.GetTranslate()); // Look at origin
	}
}
