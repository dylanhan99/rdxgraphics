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
	//auto const& handle = GetEntityHandle();
	//if (EntityManager::HasComponent<BoundingVolume::Dirty>(handle))
	//	return;
	//
	//EntityManager::AddComponent<BoundingVolume::Dirty>(handle);
}

void AABBBV::RecalculateBV()
{
	// "Original"
	Model const& model = EntityManager::GetComponent<const Model>(GetEntityHandle());
	auto& obj = RenderSystem::GetObjekt(model.GetMesh());
	AABBBV const& defaultBV = obj.GetDefaultAABBBV();

	glm::vec3 const& defaultCenter = defaultBV.GetOffset();
	glm::vec3 const& defaultHalfExtents = defaultBV.GetHalfExtents();

	Xform const& xform = EntityManager::GetComponent<const Xform>(GetEntityHandle());
	glm::vec3 const& scl = xform.GetScale();
	glm::mat4 const rot = xform.GetRotationMatrix();

	glm::vec3 newCenter{ 0.f };
	if (defaultCenter != glm::vec3{ 0.f })
		newCenter = /*glm::translate(xform.GetTranslate()) **/ rot * glm::scale(scl) * glm::vec4{ defaultCenter, 1.f };

	glm::vec3 newHalfSize{};
	for (int i = 0; i < 3; ++i)
	{
		newHalfSize[i] =
			glm::abs(rot[i][0]) * defaultHalfExtents[0] * scl[0] +
			glm::abs(rot[i][1]) * defaultHalfExtents[1] * scl[1] +
			glm::abs(rot[i][2]) * defaultHalfExtents[2] * scl[2];
	}

	GetHalfExtents() = newHalfSize;
	GetOffset() = newCenter;
}