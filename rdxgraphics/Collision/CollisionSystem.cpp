#include <pch.h>
#include "CollisionSystem.h"
#include "Entity/EntityManager.h"
#include "Utils/Input.h"

bool shite = false;

void CollisionSystem::Update(float dt)
{
	if (Input::IsKeyTriggered(GLFW_KEY_F4))
		shite = !shite;

	auto& entities = EntityManager::GetEntities();

	// Hardcoding here to un-set it for the next frame for now
	for (Entity& e : entities)
	{
		if (auto pp = e.GetColliderDetails().pBV)
			;//pp->IsCollide() = false;
	}

	for (Entity& lhs : entities)
	{
		auto& lhsCol = lhs.GetColliderDetails();
		if (!lhsCol.pBV)
			continue;
		lhsCol.pBV->IsCollide() = shite;
		for (Entity& rhs : entities)
		{
			auto& rhsCol = rhs.GetColliderDetails();
			if (!rhsCol.pBV)
				continue;

			if (lhsCol.pBV == lhsCol.pBV)
				continue;

			//bool isCollide = CheckCollision(lhsCol, rhsCol);
			//lhsCol.;
		}
	}
}
