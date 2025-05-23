#include <pch.h>
#include "Assignment1.h"
#include "GUI/GUI.h"

static float Linear(float start, float end, float value)
{
	return (1.f - value) * start + value * end;
}

void SphereXSphereScene::StartImpl()
{
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Move Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{0.f}, glm::vec3{0.5f});
		EntityManager::AddComponent<Model>(handle, Shape::Sphere);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Sphere);
		EntityManager::GetComponent<SpherePrimitive>(handle).GetRadius() = 1.5f;

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1.f;
	int step = 4.f;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "Ent ";
	for (int i = begin; i < end; ++i)
	{
		for (int j = begin; j < end; ++j)
		{
			for (int k = begin; k < end; ++k)
			{
				glm::vec3 pos{
					(float)i * step,
					(float)j * step,
					(float)k * step
				};

				std::string n = name +
					"[" + std::to_string(i + halfWidth) + "]" +
					"[" + std::to_string(j + halfWidth) + "]" +
					"[" + std::to_string(k + halfWidth) + "]";

				auto handle = BaseScene::CreateDefaultEntity();
				EntityManager::AddComponent<Metadata>(handle, n);
				EntityManager::AddComponent<Xform>(handle, pos, glm::vec3{0.5f});
				EntityManager::AddComponent<Model>(handle, Shape::Sphere);
				EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f });
				EntityManager::AddComponent<Collider>(handle, Primitive::Sphere);
			}
		}
	}
}

void AABBXSphereScene::StartImpl()
{
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Move Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f }, glm::vec3{ 0.5f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::AABB);
		EntityManager::GetComponent<AABBPrimitive>(handle).GetHalfExtents() = glm::vec3{ 1.5f };

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1.f;
	int step = 4.f;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "Ent ";
	for (int i = begin; i < end; ++i)
	{
		for (int j = begin; j < end; ++j)
		{
			for (int k = begin; k < end; ++k)
			{
				glm::vec3 pos{
					(float)i * step,
					(float)j * step,
					(float)k * step
				};

				std::string n = name +
					"[" + std::to_string(i + halfWidth) + "]" +
					"[" + std::to_string(j + halfWidth) + "]" +
					"[" + std::to_string(k + halfWidth) + "]";

				auto handle = BaseScene::CreateDefaultEntity();
				EntityManager::AddComponent<Metadata>(handle, n);
				EntityManager::AddComponent<Xform>(handle, pos, glm::vec3{ 0.5f });
				EntityManager::AddComponent<Model>(handle, Shape::Sphere);
				EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f });
				EntityManager::AddComponent<Collider>(handle, Primitive::Sphere);
			}
		}
	}
}

void AABBXAABBScene::StartImpl()
{
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Move Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f }, glm::vec3{ 0.5f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Sphere);
		EntityManager::GetComponent<AABBPrimitive>(handle).GetHalfExtents() = glm::vec3{ 1.5f };

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1.f;
	int step = 4.f;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "Ent ";
	for (int i = begin; i < end; ++i)
	{
		for (int j = begin; j < end; ++j)
		{
			for (int k = begin; k < end; ++k)
			{
				glm::vec3 pos{
					(float)i * step,
					(float)j * step,
					(float)k * step
				};

				std::string n = name +
					"[" + std::to_string(i + halfWidth) + "]" +
					"[" + std::to_string(j + halfWidth) + "]" +
					"[" + std::to_string(k + halfWidth) + "]";

				auto handle = BaseScene::CreateDefaultEntity();
				EntityManager::AddComponent<Metadata>(handle, n);
				EntityManager::AddComponent<Xform>(handle, pos, glm::vec3{ 0.5f });
				EntityManager::AddComponent<Model>(handle, Shape::Cube);
				EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f });
				EntityManager::AddComponent<Collider>(handle, Primitive::AABB);
			}
		}
	}
}

void PointXSphereScene::StartImpl()
{
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Move Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f }, glm::vec3{ 0.5f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Point);

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1.f;
	int step = 4.f;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "Ent ";
	for (int i = begin; i < end; ++i)
	{
		for (int j = begin; j < end; ++j)
		{
			for (int k = begin; k < end; ++k)
			{
				glm::vec3 pos{
					(float)i * step,
					(float)j * step,
					(float)k * step
				};

				std::string n = name +
					"[" + std::to_string(i + halfWidth) + "]" +
					"[" + std::to_string(j + halfWidth) + "]" +
					"[" + std::to_string(k + halfWidth) + "]";

				auto handle = BaseScene::CreateDefaultEntity();
				EntityManager::AddComponent<Metadata>(handle, n);
				EntityManager::AddComponent<Xform>(handle, pos, glm::vec3{ 0.5f });
				EntityManager::AddComponent<Model>(handle, Shape::Cube);
				EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f });
				EntityManager::AddComponent<Collider>(handle, Primitive::Sphere);
			}
		}
	}
}

void PointXAABBScene::StartImpl()
{
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Move Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f }, glm::vec3{ 0.5f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Point);

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1.f;
	int step = 4.f;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "Ent ";
	for (int i = begin; i < end; ++i)
	{
		for (int j = begin; j < end; ++j)
		{
			for (int k = begin; k < end; ++k)
			{
				glm::vec3 pos{
					(float)i * step,
					(float)j * step,
					(float)k * step
				};

				std::string n = name +
					"[" + std::to_string(i + halfWidth) + "]" +
					"[" + std::to_string(j + halfWidth) + "]" +
					"[" + std::to_string(k + halfWidth) + "]";

				auto handle = BaseScene::CreateDefaultEntity();
				EntityManager::AddComponent<Metadata>(handle, n);
				EntityManager::AddComponent<Xform>(handle, pos, glm::vec3{ 0.5f });
				EntityManager::AddComponent<Model>(handle, Shape::Cube);
				EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f });
				EntityManager::AddComponent<Collider>(handle, Primitive::AABB);
			}
		}
	}
}

void PointXTriangleScene::StartImpl()
{
}

void PointXPlaneScene::StartImpl()
{
}

void RayXSphereScene::StartImpl()
{
}

void RayXAABBScene::StartImpl()
{
}

void RayXTriangleScene::StartImpl()
{
}

void RayXPlaneScene::StartImpl()
{
}

void PlaneXAABBScene::StartImpl()
{
}

void PlaneXSphereScene::StartImpl()
{
}
