#include <pch.h>
#include "BaseComponent.h"
#include "EntityManager.h"

void Xform::UpdateXform()
{
	m_Xform = glm::translate(m_Translate) * glm::scale(m_Scale);// *glm::rotate(glm::quat(m_Rotate));
}

void Collider::SetBV(BV bvType)
{
	if (GetBVType() != BV::NIL)
	{
		// Remove the old one,
	}
	
	m_BVType = bvType;
	switch (m_BVType)
	{
	case BV::AABB:
		EntityManager::AddComponent<AABBBV>(m_Handle);
		break;
	default:
		// do nothing for now
		m_BVType = BV::NIL;
		return;
	}
}
