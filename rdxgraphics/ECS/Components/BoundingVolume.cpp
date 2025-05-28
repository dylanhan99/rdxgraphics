#include <pch.h>
#include "BoundingVolume.h"
#include "ECS/EntityManager.h"
#include "ECS/Systems/RenderSystem.h"

void BoundingVolume::SetBVType(BV bvType)
{
	if (m_BVType == bvType)
		return;

	glm::vec3 offset = RemoveBV();
	m_BVType = bvType;
	if (m_BVType == BV::NIL)
		return;

	SetupBV(offset);
}

void BoundingVolume::SetupBV(glm::vec3 offset)
{
	// Get the BV& component and 
	entt::entity const handle = GetEntityHandle();
#define _RX_X(Klass)
	switch (m_BVType)
	{
	case BV::AABB:
		EntityManager::AddComponent<AABBBV>(handle);
		RX_ASSERT(EntityManager::HasComponent<AABBBV>(handle));
		//EntityManager::GetComponent<AABBBV
		break;
	default:
		RX_ASSERT(false, "How did you get here");
		break;
	}
#undef _RX_X
}

glm::vec3 BoundingVolume::RemoveBV()
{
	m_BVType = BV::NIL;
	return glm::vec3();
}

void BaseBV::SetDirty() const
{
	auto const& handle = GetEntityHandle();
	if (EntityManager::HasComponent<BoundingVolume::Dirty>(handle))
		return;

	EntityManager::AddComponent<BoundingVolume::Dirty>(handle);
}

void AABBBV::RecalculateBV()
{
	auto const& handle = GetEntityHandle();
	if (!EntityManager::HasComponent<Model>(handle))
		return;

	Model& model = EntityManager::GetComponent<Model>(handle);
	auto& object = RenderSystem::GetObjekt(model.GetMesh());
	auto& vertexPositions = object.GetVBData<VertexBasic::Position>();


}