#include <pch.h>
#include "Collider.h"
#include "ECS/EntityManager.h"
#include "ECS/Components.h"

void Collider::SetPrimitiveType(Primitive primType)
{
	if (m_PrimitiveType == primType)
		return;

	glm::vec3 prevPos = RemovePrimitive(); // Remove the existing BV first
	m_PrimitiveType = primType;
	if (primType == Primitive::NIL)
		return;

	SetupPrimitive(prevPos);
}

void Collider::SetupPrimitive(glm::vec3 pos) const
{
	entt::entity const handle = GetEntityHandle();
#define _RX_X(Klass)															\
	case Primitive::Klass:														\
		EntityManager::AddComponent<Klass##Primitive>(handle);					\
		RX_ASSERT(EntityManager::HasComponent<Klass##Primitive>(handle));		\
		EntityManager::GetComponent<Klass##Primitive>(handle).SetPosition(pos);	\
		break;

	switch (m_PrimitiveType)
	{
		RX_DO_ALL_BV_ENUM;
	default:
		RX_ASSERT(false, "How did you get here");
		break;
	}
#undef _RX_X
}

glm::vec3 Collider::RemovePrimitive()
{
	if (m_PrimitiveType == Primitive::NIL)
		return {};

	entt::entity const handle = GetEntityHandle();
	glm::vec3 pos{};
#define _RX_X(Klass)																\
	case Primitive::Klass:															\
		RX_ASSERT(EntityManager::HasComponent<Klass##Primitive>(handle));			\
		pos = EntityManager::GetComponent<Klass##Primitive>(handle).GetPosition();	\
		EntityManager::RemoveComponent<Klass##Primitive>(handle);					\
		break;

	switch (m_PrimitiveType)
	{
		RX_DO_ALL_BV_ENUM;
	default:
		RX_ASSERT(false, "How did you get here");
		break;
	}
#undef _RX_X

	m_PrimitiveType = Primitive::NIL;
	return pos;
}
