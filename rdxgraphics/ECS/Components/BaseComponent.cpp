#include <pch.h>
#include "BaseComponent.h"
#include "ECS/EntityManager.h"

void Xform::UpdateXform()
{
	m_Xform = glm::translate(m_Translate) * glm::scale(m_Scale) * static_cast<glm::mat4>(glm::quat(m_Rotate));
}

void Collider::SetBV(BV bvType)
{
	if (m_BVType == bvType)
		return;

	RemoveBV(); // Remove the existing BV first
	m_BVType = bvType;
	
#define _RX_X(Klass)									 \
	case BV::Klass:										 \
		EntityManager::AddComponent<Klass##BV>(m_Handle);\
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

void Collider::RemoveBV()
{
	if (m_BVType == BV::NIL)
		return;

#define _RX_X(Klass)												\
	case BV::Klass:													\
		RX_ASSERT(EntityManager::HasComponent<Klass##BV>(m_Handle));\
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
}
