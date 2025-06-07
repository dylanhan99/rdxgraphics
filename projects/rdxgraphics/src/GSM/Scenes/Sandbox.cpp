#include "Sandbox.h"
#include "Utils/Input.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/EntityManager.h"
#include "ECS/Components/Camera.h"
#include "GUI/GUI.h"

void Sandbox::StartImpl()
{
	entt::entity test{};
	//{
	//	auto handle = BaseScene::CreateDefaultEntity<NoDelete>();
	//	EntityManager::AddComponent<Metadata>(handle, "L");
	//	EntityManager::AddComponent<Xform>(handle, glm::vec3{ 1.f, 1.f, 1.f }, glm::vec3{ 1.f }, glm::vec3{ glm::quarter_pi<float>() });
	//	EntityManager::AddComponent<Model>(handle, Shape::Sphere);
	//	EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f,1.f,0.f });
	//	EntityManager::AddComponent<Collider>(handle, Primitive::AABB);
	//	return;
	//}
	{
		auto handle = test = BaseScene::CreateDefaultEntity<NoDelete>();
		EntityManager::AddComponent<Metadata>(handle, "L");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 1.f }, glm::vec3{ 0.f })
			;
		//EntityManager::AddComponent<Model>(handle, Rxuid("ogre"));
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f,1.f,0.f });
		//EntityManager::AddComponent<BoundingVolume>(handle, BV::Sphere);
		//EntityManager::AddComponent<Collider>(handle, Primitive::Plane);
		GUI::SetSelectedEntity(handle);
	}
	{
		auto clone = BaseScene::CloneEntity(test);
		EntityManager::GetComponent<Xform>(clone)
			.SetTranslate({-2.f, 0.f, -2.f});
		EntityManager::GetComponent<Model>(clone)
			.SetMesh(Shape::Cube);
		EntityManager::AddComponent<Collider>(clone, Primitive::AABB);
		EntityManager::AddComponent<BoundingVolume>(clone, BV::AABB);
	}
	{
		auto clone = BaseScene::CloneEntity(test);
		EntityManager::GetComponent<Xform>(clone)
			.SetTranslate({2.f, 0.f, 2.f});
		EntityManager::GetComponent<Model>(clone)
			.SetMesh(Rxuid{ "ogre" });
		EntityManager::AddComponent<Collider>(clone, Primitive::Plane);
		EntityManager::AddComponent<BoundingVolume>(clone, BV::OBB);
	}
		EntityManager::AddComponent<BoundingVolume>(test, BV::Sphere);
}

void Sandbox::UpdateImpl(float dt)
{
}
