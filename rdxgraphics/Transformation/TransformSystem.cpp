#include <pch.h>
#include "TransformSystem.h"
#include "ECS/EntityManager.h"

RX_SINGLETON_EXPLICIT(TransformSystem);

void TransformSystem::Update(float dt)
{
	auto xformView = EntityManager::GetInstance().m_Registry.view<Xform>();
	for (auto [handle, xform] : xformView.each())
	{
		xform.UpdateXform();
	}

	auto colView = EntityManager::GetInstance().m_Registry.view<const Collider>();
	for (auto [handle, col] : colView.each())
	{
		if (col.GetBVType() == BV::NIL)
			continue;

		// Hardcode to follow xform for now?
		{
			Xform& xform = EntityManager::GetInstance().m_Registry.get<Xform>(handle);
#define _RX_X(Klass)																		\
		case BV::Klass:																		\
		{																					\
			Klass##BV& bv = EntityManager::GetInstance().m_Registry.get<Klass##BV>(handle); \
			bv.GetPosition() = xform.GetTranslate();										\
	} break;

			switch (col.GetBVType())
			{
				RX_DO_ALL_BV_ENUM;
			default:
				break;
			}
#undef _RX_X
		}

#define _RX_X(Klass)																	\
	case BV::Klass:																		\
	{																					\
		/*Should check ensure that get<BV> exists*/										\
		Klass##BV& bv = EntityManager::GetInstance().m_Registry.get<Klass##BV>(handle); \
		bv.UpdateXform();																\
	} break;

		switch (col.GetBVType())
		{
			RX_DO_ALL_BV_ENUM;
		default:
			break;
		}
#undef _RX_X
	}
}
