#include <pch.h>
#include "TransformSystem.h"
#include <Entity/EntityManager.h>

RX_SINGLETON_EXPLICIT(TransformSystem);

void TransformSystem::Update(float dt)
{
	for (Entity& ent : EntityManager::GetEntities())
	{
		if (auto pBV = ent.GetColliderDetails().pBV)
			pBV->GetPosition() = ent.GetModelDetails().Translate; // Lazily having col and model's position be the same

		ent.GetModelDetails().UpdateXform();
		ent.GetColliderDetails().UpdateXform();
	}
}
