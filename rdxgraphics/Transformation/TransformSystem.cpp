#include <pch.h>
#include "TransformSystem.h"
#include <Entity/EntityManager.h>

RX_SINGLETON_EXPLICIT(TransformSystem);

void TransformSystem::Update(float dt)
{
	for (Entity& ent : EntityManager::GetEntities())
	{
		ent.GetModelDetails().UpdateXform();
		ent.GetColliderDetails().UpdateXform();
	}
}
