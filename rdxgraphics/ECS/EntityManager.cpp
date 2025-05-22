#include <pch.h>
#include "EntityManager.h"

RX_SINGLETON_EXPLICIT(EntityManager);

bool EntityManager::Init()
{
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
