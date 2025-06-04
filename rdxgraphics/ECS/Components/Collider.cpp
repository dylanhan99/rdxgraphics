#include <pch.h>
#include "Collider.h"
#include "ECS/EntityManager.h"
#include "ECS/Components.h"

void Collider::SetPrimitiveType(Primitive primType)
{
	if (m_PrimitiveType == primType)
		return;

	glm::vec3 offset = RemovePrimitive(); // Remove the existing BV first
	m_PrimitiveType = primType;
	if (primType == Primitive::NIL)
		return;

	SetupPrimitive(offset);
}

void Collider::SetupPrimitive(glm::vec3 offset) const
{
	entt::entity const handle = GetEntityHandle();
#define _RX_X(Klass)															\
	case Primitive::Klass:														\
		EntityManager::AddComponent<Klass##Primitive>(handle);					\
		RX_ASSERT(EntityManager::HasComponent<Klass##Primitive>(handle));		\
		EntityManager::GetComponent<Klass##Primitive>(handle).SetOffset(offset);\
		break;

	switch (m_PrimitiveType)
	{
		RX_DO_ALL_PRIMITIVE_ENUM;
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
	glm::vec3 o{};
#define _RX_X(Klass)															 \
	case Primitive::Klass:														 \
		RX_ASSERT(EntityManager::HasComponent<Klass##Primitive>(handle));		 \
		o = EntityManager::GetComponent<Klass##Primitive>(handle).GetOffset();	 \
		EntityManager::RemoveComponent<Klass##Primitive>(handle);				 \
		break;

	switch (m_PrimitiveType)
	{
		RX_DO_ALL_PRIMITIVE_ENUM;
	default:
		RX_ASSERT(false, "How did you get here");
		break;
	}
#undef _RX_X

	m_PrimitiveType = Primitive::NIL;
	return o;
}

glm::vec3 BasePrimitive::GetPosition() const
{
	return EntityManager::GetComponent<const Xform>(GetEntityHandle()).GetTranslate() + m_Offset;
}

void BasePrimitive::SetPosition(glm::vec3 pos)
{
	SetDirtyXform();
	glm::vec3 position = EntityManager::GetComponent<const Xform>(GetEntityHandle()).GetTranslate();
	m_Offset = pos - position;
}

void BasePrimitive::SetDirtyXform() const
{
	auto const& handle = GetEntityHandle();
	if (!EntityManager::HasEntity(handle))
		return;
	if (EntityManager::HasComponent<Collider::DirtyXform>(handle))
		return;

	EntityManager::AddComponent<Collider::DirtyXform>(handle);
}
