#include "Assignment2.h"
#include "GUI/GUI.h"

void Assignment2::StartImpl()
{
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Bunny");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f, 0.f, -2.f }, glm::vec3{ 10.f }, glm::vec3{ -glm::quarter_pi<float>() });
		EntityManager::AddComponent<Model>(handle, Rxuid{"bunny"});
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 1.f, 0.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Sphere);
		EntityManager::AddComponent<BoundingVolume>(handle, BV::Sphere);
	}
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Cup");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 4.f, 0.f, -2.f }, glm::vec3{ 10.f }, glm::vec3{ glm::quarter_pi<float>() });
		EntityManager::AddComponent<Model>(handle, Rxuid{"cup"});
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 1.f, 0.f });
		EntityManager::AddComponent<BoundingVolume>(handle, BV::Sphere);
	}
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Starwars");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 158.f, 0.f, -414.f }, glm::vec3{ 1.f }, glm::vec3{ 0.22f, 0.f, 0.1f });
		EntityManager::AddComponent<Model>(handle, Rxuid{"starwars1"});
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f, 0.9f, 0.f });
		EntityManager::AddComponent<BoundingVolume>(handle, BV::OBB);
	}
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Lucy Princeton");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f, 0.f, -20.f }, glm::vec3{ 0.01f }, glm::vec3{ glm::quarter_pi<float>() });
		EntityManager::AddComponent<Model>(handle, Rxuid{"lucy_princeton"});
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f, 1.f, 1.f });
		EntityManager::AddComponent<BoundingVolume>(handle, BV::OBB);
	}
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Rhino");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ -16.f, 0.f, 16.f }, glm::vec3{ 1.f }, glm::vec3{0.f, 0.f, glm::half_pi<float>()});
		EntityManager::AddComponent<Model>(handle, Rxuid{"rhino"});
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f, 1.f, 1.f });
		EntityManager::AddComponent<BoundingVolume>(handle, BV::Sphere);
	}
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Spinny Cube");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 1.f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f, 1.f, 1.f });
		EntityManager::AddComponent<BoundingVolume>(handle, BV::AABB);

		m_Spinny = handle;
	}

	GUI::SetSelectedEntity(m_Spinny);
}

void Assignment2::UpdateImpl(float dt)
{
	if (EntityManager::HasEntity(m_Spinny))
	{
		glm::vec3 rate{
			glm::quarter_pi<float>(),
			glm::quarter_pi<float>() * 0.5f,
			-glm::half_pi<float>()
		};
		rate *= dt;

		Xform& xform = EntityManager::GetComponent<Xform>(m_Spinny);
		glm::vec3& euler = xform.GetEulerOrientation();
		euler += rate;

		EntityManager::GetComponent<BoundingVolume>(m_Spinny).SetDirty();
	}
}