#include <pch.h>
#include "TransformSystem.h"
#include "ECS/EntityManager.h"

RX_SINGLETON_EXPLICIT(TransformSystem);

void TransformSystem::Update(float dt)
{
	static std::set<entt::entity> seen{};
	auto xformView = EntityManager::GetInstance().m_Registry.view<Xform>();
	for (auto [handle, xform] : xformView.each())
	{
		xform.UpdateXform();
		seen.insert(handle);
	}

	auto colView = EntityManager::GetInstance().m_Registry.view<const Collider>();
	for (auto [handle, col] : colView.each())
	{
		//if (seen.find(handle) != seen.end())
		//	continue;
		//if (col.GetBVType() == BV::NIL)
		//	continue;

		// hardcode test
		if (col.GetBVType() == BV::AABB)
		{
			// Should check ensure that get<BV> exists
			AABBBV& bv = EntityManager::GetInstance().m_Registry.get<AABBBV>(handle);
			bv.UpdateXform();
		}
	}
	seen.clear();
}
