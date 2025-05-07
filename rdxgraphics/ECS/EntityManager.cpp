#include <pch.h>
#include "EntityManager.h"
#include <entt/entt.hpp>

RX_SINGLETON_EXPLICIT(EntityManager);

void Entity::ModelDetails::UpdateXform()
{
	Xform = glm::mat4(glm::translate(Translate));
}

bool EntityManager::Init()
{
	return false;
}

void EntityManager::Terminate()
{
}
