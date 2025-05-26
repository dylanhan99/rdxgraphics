#include <pch.h>
#include "EntityManager.h"
#include "Components.h"

RX_SINGLETON_EXPLICIT(EntityManager);

bool EntityManager::Init()
{
	Xform::Init(g.m_Registry);
	Camera::Init(g.m_Registry);
	Collider::Init(g.m_Registry);

#define _RX_X(Klass) Klass##Primitive::Init(g.m_Registry);
	RX_DO_ALL_BV_ENUM;
#undef _RX_X

	return true;
}

void EntityManager::Terminate()
{
	g.m_Registry.clear();
}

bool EntityManager::HasEntity(entt::entity handle)
{
	return g.m_Registry.valid(handle);
}
