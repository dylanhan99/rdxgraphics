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

	VertexBasic::Position::attrib_type min{std::numeric_limits<float>::max()}, max{ std::numeric_limits<float>::min() };
	for (VertexBasic::Position::attrib_type const& pos : vertexPositions)
	{
		min.x = (pos.x < min.x) ? pos.x : min.x;
		min.y = (pos.y < min.y) ? pos.y : min.y;
		min.z = (pos.z < min.z) ? pos.z : min.z;

		max.x = (pos.x > max.x) ? pos.x : max.x;
		max.y = (pos.y > max.y) ? pos.y : max.y;
		max.z = (pos.z > max.z) ? pos.z : max.z;
	}

	SetOffset((max + min) * 0.5f);
	GetHalfExtents() = glm::abs((max - min) * 0.5f);
}