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
		EntityManager::AddComponent<Metadata>(handle, "(Sphere) Move Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{0.f}, glm::vec3{0.5f});
		EntityManager::AddComponent<Model>(handle, Shape::Sphere);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Sphere);
		EntityManager::GetComponent<SpherePrimitive>(handle).GetRadius() = 1.5f;

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1;
	int step = 4;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "Sphere ";
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
		EntityManager::AddComponent<Metadata>(handle, "(AABB) Move Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f }, glm::vec3{ 0.5f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::AABB);
		EntityManager::GetComponent<AABBPrimitive>(handle).GetHalfExtents() = glm::vec3{ 1.5f };

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1;
	int step = 4;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "Sphere ";
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
		EntityManager::AddComponent<Metadata>(handle, "(AABB) Move Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f }, glm::vec3{ 0.5f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Sphere);
		EntityManager::GetComponent<AABBPrimitive>(handle).GetHalfExtents() = glm::vec3{ 1.5f };

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1;
	int step = 4;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "AABB ";
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
		EntityManager::AddComponent<Metadata>(handle, "(Point) Move Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f }, glm::vec3{ 0.5f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Point);

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1;
	int step = 4;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "Sphere ";
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
		EntityManager::AddComponent<Metadata>(handle, "(Point) Move Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f }, glm::vec3{ 0.5f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Point);

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1;
	int step = 4;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "AABB ";
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
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "(Triangle) Move my points!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f }, glm::vec3{ 0.5f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Triangle);
		auto& prim = EntityManager::GetComponent<TrianglePrimitive>(handle);

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1;
	int step = 4;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "Point ";
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
				EntityManager::AddComponent<Xform>(handle, pos, glm::vec3{ 0.25f });
				EntityManager::AddComponent<Model>(handle, Shape::Cube);
				EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f });
				EntityManager::AddComponent<Collider>(handle, Primitive::Point);
			}
		}
	}
}

void PointXPlaneScene::StartImpl()
{
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "(Plane) Spin Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f }, glm::vec3{ 0.5f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Plane);

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1;
	int step = 4;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "Point ";
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
				EntityManager::AddComponent<Xform>(handle, pos, glm::vec3{ 0.25f });
				EntityManager::AddComponent<Model>(handle, Shape::Cube);
				EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f });
				EntityManager::AddComponent<Collider>(handle, Primitive::Point);
			}
		}
	}
}

void RayXSphereScene::StartImpl()
{
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "(Ray) Point Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f }, glm::vec3{ 0.5f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Ray);

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1;
	int step = 4;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "Sphere ";
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
				EntityManager::AddComponent<Xform>(handle, pos, glm::vec3{ 0.25f });
				EntityManager::AddComponent<Model>(handle, Shape::Sphere);
				EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f });
				EntityManager::AddComponent<Collider>(handle, Primitive::Sphere);
			}
		}
	}
}

void RayXAABBScene::StartImpl()
{
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "(Ray) Point Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f }, glm::vec3{ 0.5f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Ray);

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1;
	int step = 4;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "AABB ";
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
				EntityManager::AddComponent<Xform>(handle, pos, glm::vec3{ 0.25f });
				EntityManager::AddComponent<Model>(handle, Shape::Cube);
				EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f });
				EntityManager::AddComponent<Collider>(handle, Primitive::AABB);
			}
		}
	}
}

void RayXTriangleScene::StartImpl()
{
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "(Ray) Point Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f }, glm::vec3{ 0.5f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Ray);

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1;
	int step = 4;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "Triangle ";
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
				EntityManager::AddComponent<Xform>(handle, pos, glm::vec3{ 0.25f });
				EntityManager::AddComponent<Model>(handle, Shape::Triangle);
				EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f });
				EntityManager::AddComponent<Collider>(handle, Primitive::Triangle);
			}
		}
	}
}

void RayXPlaneScene::StartImpl()
{
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "(Ray) Point Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f }, glm::vec3{ 0.5f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Ray);

		GUI::SetSelectedEntity(handle);
	}

	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Plane (Top)");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f, 10.f, -5.f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Plane);
		auto& prim = EntityManager::GetComponent<PlanePrimitive>(handle);
		prim.GetOrientation() = { -glm::half_pi<float>(), 0.f, 0.f};
	}
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Plane (Bottom)");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f, -10.f, -5.f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Plane);
		auto& prim = EntityManager::GetComponent<PlanePrimitive>(handle);
		prim.GetOrientation() = { glm::half_pi<float>(), 0.f, 0.f};
	}
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Plane (Left)");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ -10.f, 0.f, -5.f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Plane);
		auto& prim = EntityManager::GetComponent<PlanePrimitive>(handle);
		prim.GetOrientation() = { 0.f, glm::half_pi<float>(), 0.f};
	}
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "Plane (Right)");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 10.f, 0.f, -5.f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Plane);
		auto& prim = EntityManager::GetComponent<PlanePrimitive>(handle);
		prim.GetOrientation() = { 0.f, -glm::half_pi<float>(), 0.f};
	}
}

void PlaneXAABBScene::StartImpl()
{
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "(Plane) Spin Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f }, glm::vec3{ 0.5f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Plane);

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1;
	int step = 4;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "AABB ";
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
				EntityManager::AddComponent<Xform>(handle, pos, glm::vec3{ 0.25f });
				EntityManager::AddComponent<Model>(handle, Shape::Cube);
				EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f });
				EntityManager::AddComponent<Collider>(handle, Primitive::AABB);
			}
		}
	}
}

void PlaneXSphereScene::StartImpl()
{
	{
		auto handle = BaseScene::CreateDefaultEntity();
		EntityManager::AddComponent<Metadata>(handle, "(Plane) Spin Me!");
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f }, glm::vec3{ 0.5f });
		EntityManager::AddComponent<Model>(handle, Shape::Cube);
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 0.f, 1.f });
		EntityManager::AddComponent<Collider>(handle, Primitive::Plane);

		GUI::SetSelectedEntity(handle);
	}


	int halfWidth = 1;
	int step = 4;

	int begin = -halfWidth;
	int end = halfWidth;

	std::string name = "Sphere ";
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
				EntityManager::AddComponent<Xform>(handle, pos, glm::vec3{ 0.25f });
				EntityManager::AddComponent<Model>(handle, Shape::Sphere);
				EntityManager::AddComponent<Material>(handle, glm::vec3{ 1.f });
				EntityManager::AddComponent<Collider>(handle, Primitive::Sphere);
			}
		}
	}
}
