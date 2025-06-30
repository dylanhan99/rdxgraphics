#include "Assignment3.h"
#include "GUI/GUI.h"
#include "ECS/Systems/BVHSystem.h"

void Assignment3::StartImpl()
{
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Bunny");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f, 0.f, -2.f }, glm::vec3{ 10.f }, glm::vec3{ -glm::quarter_pi<float>() });
		EntityManager::AddComponent<Model>(handle, Rxuid{"bunny"});
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 1.f, 0.f });
		EntityManager::AddComponent<BoundingVolume>(handle, BV::AABB);
	}
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Cup");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ -10.f, 0.f, -2.f }, glm::vec3{ 10.f }, glm::vec3{ glm::quarter_pi<float>() });
		EntityManager::AddComponent<Model>(handle, Rxuid{"cup"});
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 1.f, 0.f });
		EntityManager::AddComponent<BoundingVolume>(handle, BV::AABB);
	}
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Starwars");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ -10.f, 0.f, -2.f }, glm::vec3{ 0.01f }, glm::vec3{ 0.22f, 0.f, 0.1f });
		EntityManager::AddComponent<Model>(handle, Rxuid{"starwars1"});
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f, 0.9f, 0.f });
		EntityManager::AddComponent<BoundingVolume>(handle, BV::AABB);
	}
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Lucy Princeton");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 4.f, -1.f, 2.f }, glm::vec3{ 0.001f }, glm::vec3{ glm::quarter_pi<float>() });
		EntityManager::AddComponent<Model>(handle, Rxuid{"lucy_princeton"});
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f, 1.f, 1.f });
		EntityManager::AddComponent<BoundingVolume>(handle, BV::AABB);
	}
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Rhino");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ -6.f, 2.f, 2.f }, glm::vec3{ 0.1f }, glm::vec3{0.f, glm::quarter_pi<float>(), glm::half_pi<float>()});
		EntityManager::AddComponent<Model>(handle, Rxuid{"rhino"});
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f, 1.f, 1.f });
		EntityManager::AddComponent<BoundingVolume>(handle, BV::AABB);
	}
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Spinny Cube");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 1.f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f, 1.f, 1.f });
		EntityManager::AddComponent<BoundingVolume>(handle, BV::AABB);
	}

	BVHSystem::EnforceUniformBVs();
}

void Assignment3::UpdateImpl(float dt)
{
	BVHSystem::BuildBVH();
}