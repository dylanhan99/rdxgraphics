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

void EntityManager::ExileEntity(entt::entity handle)
{
	g.m_Registry.destroy(handle);
}

bool EntityManager::HasEntity(entt::entity handle)
{
	return g.m_Registry.valid(handle);
}
