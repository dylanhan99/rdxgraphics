#include <pch.h>
#include "Collider.h"
#include "ECS/EntityManager.h"
#include "ECS/Components.h"

void Collider::SetBV(BV bvType)
{
	if (m_BVType == bvType)
		return;

	glm::vec3 prevPos = RemoveBV(); // Remove the existing BV first
	m_BVType = bvType;
	if (bvType == BV::NIL)
		return;

#define _RX_X(Klass)									 \
	case BV::Klass:										 \
		EntityManager::AddComponent<Klass##BV>(m_Handle);\
		RX_ASSERT(EntityManager::HasComponent<Klass##BV>(m_Handle));\
		EntityManager::GetComponent<Klass##BV>(m_Handle).SetPosition(prevPos);\
		break;

	switch (m_BVType)
	{
		RX_DO_ALL_BV_ENUM;
	default:
		RX_ASSERT(false, "How did you get here");
		break;
	}
#undef _RX_X
}

glm::vec3 Collider::RemoveBV()
{
	if (m_BVType == BV::NIL)
		return {};

	glm::vec3 pos{};
#define _RX_X(Klass)												\
	case BV::Klass:													\
		RX_ASSERT(EntityManager::HasComponent<Klass##BV>(m_Handle));\
		pos = EntityManager::GetComponent<Klass##BV>(m_Handle).GetPosition();\
		EntityManager::RemoveComponent<Klass##BV>(m_Handle);		\
		break;

	switch (m_BVType)
	{
		RX_DO_ALL_BV_ENUM;
	default:
		RX_ASSERT(false, "How did you get here");
		break;
	}
#undef _RX_X

	m_BVType = BV::NIL;
	return pos;
}
