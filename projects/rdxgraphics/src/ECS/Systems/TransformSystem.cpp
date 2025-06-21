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

		if (EntityManager::HasComponent<Collider>(handle))
		{
			auto const& col = EntityManager::GetComponent<const Collider>(handle);
			switch (col.GetPrimitiveType())
			{
				RX_DO_ALL_PRIMITIVE_ENUM;
			default:
				break;
			}
			EntityManager::RemoveComponent<Collider::DirtyXform>(handle);
		}

		if (EntityManager::HasComponent<BoundingVolume::DirtyXform>(handle) &&
			!EntityManager::HasComponent<BoundingVolume::DirtyBV>(handle))
		{
#define _RX_XX(Klass)											 \
	case BV::Klass:												 \
	{															 \
		Klass##BV& bv = EntityManager::GetComponent<Klass##BV>(handle);\
		bv.UpdateXform();										 \
		break;													 \
	}

			switch (EntityManager::GetComponent<BoundingVolume>(handle).GetBVType())
			{
				RX_DO_ALL_BV_ENUM_M(_RX_XX);
			default:
				break;
			}
#undef _RX_XX

			EntityManager::RemoveComponent<BoundingVolume::DirtyXform>(handle);
		}
	}

	// Handles dirty colliders if somehow not handled above already.
	{
		auto v = EntityManager::View<Collider::DirtyXform, Xform, Collider>();
		for (auto [handle, _, xform, col] : v.each())
		{
			auto [xform, col] = EntityManager::GetComponent<const Xform, const Collider>(handle);
			switch (col.GetPrimitiveType())
			{
				RX_DO_ALL_PRIMITIVE_ENUM;
			default:
				break;
			}
			EntityManager::RemoveComponent<Collider::DirtyXform>(handle);
		}
	}
#undef _RX_X

	// handling dirty BVs
	{
		auto v = EntityManager::View<BoundingVolume::DirtyBV, BoundingVolume>();
		for (auto [handle, _, boundingVolume] : v.each())
		{
			boundingVolume.RecalculateBV();

			EntityManager::RemoveComponent<BoundingVolume::DirtyXform>(handle);
			EntityManager::RemoveComponent<BoundingVolume::DirtyBV>(handle);
		}
	}
}
