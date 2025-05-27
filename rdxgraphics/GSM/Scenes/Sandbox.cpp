#include <pch.h>
#include "Sandbox.h"
#include "Utils/Input.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/EntityManager.h"
#include "ECS/Components/Camera.h"

void Sandbox::StartImpl()
{
	{
		auto handle = BaseScene::CreateDefaultEntity<NoDelete>();
		EntityManager::AddComponent<Metadata>(handle, "L");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 1.f, 1.f, 1.f }, glm::vec3{ 1.f }, glm::vec3{ glm::quarter_pi<float>() });
		EntityManager::AddComponent<Model>(handle, Rxuid("ogre"));
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f,1.f,0.f });
	}
	{
		auto handle = BaseScene::CreateDefaultEntity<NoDelete>();
		EntityManager::AddComponent<Metadata>(handle, "L");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 1.f }, glm::vec3{ glm::quarter_pi<float>() });
		EntityManager::AddComponent<Model>(handle, Rxuid("starwars1"));
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f,0.f,1.f });
	}
	{
		auto handle = BaseScene::CreateDefaultEntity<NoDelete>();
		EntityManager::AddComponent<Metadata>(handle, "L");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 5.f, 5.f, 5.f }, glm::vec3{ 1.f }, glm::vec3{ glm::quarter_pi<float>() });
		EntityManager::AddComponent<Model>(handle, Rxuid("head"));
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f,0.f,1.f });
	}
}

void Sandbox::UpdateImpl(float dt)
{
}
