#include <pch.h>
#include "EntityManager.h"
#include "Components.h"

RX_SINGLETON_EXPLICIT(EntityManager);

bool EntityManager::Init()
{
	Xform::Init(g.m_Registry);
	Camera::Init(g.m_Registry);
	Collider::Init(g.m_Registry);
	BoundingVolume::Init(g.m_Registry);

#define _RX_X(Klass) Klass##Primitive::Init(g.m_Registry);
	RX_DO_ALL_PRIMITIVE_ENUM;
#undef _RX_X

	return true;
}

void EntityManager::Terminate()
{
	g.m_Registry.clear();
}

entt::entity EntityManager::CloneEntity(entt::entity handle)
{
	if (!EntityManager::HasEntity(handle))
		return entt::null;

	// for each component, copy it into new fella
	entt::entity clone = CreateEntity();

#define _RX_X(Klass)																		  \
	if (EntityManager::HasComponent<Klass>(handle))											  \
	{																						  \
		EntityManager::AddComponent<Klass>(clone, EntityManager::GetComponent<Klass>(handle));\
	}
	RX_DO_MAIN_COMPONENTS;
#undef _RX_X

#define _RX_X(Klass)																								\
	if (EntityManager::HasComponent<Klass##Primitive>(handle))														\
	{																												\
		EntityManager::AddComponent<Klass##Primitive>(clone, EntityManager::GetComponent<Klass##Primitive>(handle));\
	}
	RX_DO_ALL_PRIMITIVE_ENUM;
#undef _RX_X

	return clone;
}

bool EntityManager::HasEntity(entt::entity handle)
{
	return g.m_Registry.valid(handle);
}
