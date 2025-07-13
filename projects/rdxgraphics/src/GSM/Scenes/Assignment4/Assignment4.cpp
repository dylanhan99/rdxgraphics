#include "Assignment4.h"

void Assignment4::StartImpl()
{
#define _RX_X(name, path)														   \
	{																			   \
		auto handle = BaseScene::CreateDefaultEntity();							   \
		EntityManager::AddComponent<Metadata>(handle, name);					   \
		EntityManager::AddComponent<Xform>(handle, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{0.0005f});	   \
		EntityManager::AddComponent<Model>(handle, Rxuid{ path });				   \
		EntityManager::AddComponent<Material>(handle, glm::vec3{ 0.f, 1.f, 0.f }); \
		EntityManager::AddComponent<BoundingVolume>(handle, BV::AABB);			   \
	}

	RX_DO_ALL_UNC_M(_RX_X);
#undef _RX_X

	BVHSystem::EnforceUniformBVs();
}

void Assignment4::UpdateImpl(float dt)
{
	//BVHSystem::BuildBVH();
}