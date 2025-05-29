#include <pch.h>
#include "TransformSystem.h"
#include "ECS/EntityManager.h"
#include "ECS/Components.h"

RX_SINGLETON_EXPLICIT(TransformSystem);

void TransformSystem::Update(float dt)
{
#define _RX_X(Klass)													 \
case Primitive::Klass:													 \
{																		 \
	/*Should check ensure that get<BV> exists*/							 \
	RX_ASSERT(EntityManager::HasComponent<Klass##Primitive>(handle));	 \
	EntityManager::GetComponent<Klass##Primitive>(handle).UpdateXform(); \
} break;

	// Handles a mix of dirty xform and dirty colliders
	auto xformView = EntityManager::View<Xform::Dirty, Xform>();
	for (auto [handle, _, xform] : xformView.each())
	{
		xform.UpdateXform();
		EntityManager::RemoveComponent<Xform::Dirty>(handle);

		if (!EntityManager::HasComponent<Collider>(handle))
			continue;

		auto const& col = EntityManager::GetComponent<const Collider>(handle);
		switch (col.GetPrimitiveType())
		{
			RX_DO_ALL_PRIMITIVE_ENUM;
		default:
			break;
		}
		EntityManager::RemoveComponent<Collider::Dirty>(handle);
	}

	// Handles dirty colliders if somehow not handled above already.
	{
		auto v = EntityManager::View<Collider::Dirty, Xform, Collider>();
		for (auto [handle, _, xform, col] : v.each())
		{
			auto [xform, col] = EntityManager::GetComponent<const Xform, const Collider>(handle);
			switch (col.GetPrimitiveType())
			{
				RX_DO_ALL_PRIMITIVE_ENUM;
			default:
				break;
			}
			EntityManager::RemoveComponent<Collider::Dirty>(handle);
		}
	}
#undef _RX_X

	// handling dirty BVs
	{
		auto v = EntityManager::View<BoundingVolume::Dirty>();
		for (auto [handle, _] : v.each())
		{
			auto& boundingVolume = EntityManager::GetComponent<BoundingVolume>(handle);
			switch (boundingVolume.GetBVType())
			{
			case BV::AABB:
			{
				AABBBV& bv = EntityManager::GetComponent<AABBBV>(handle);
				bv.RecalculateBV();
				bv.UpdateXform();
				break;
			}
			default:
				break;
			}

			EntityManager::RemoveComponent<BoundingVolume::Dirty>(handle);
		}
	}
}
