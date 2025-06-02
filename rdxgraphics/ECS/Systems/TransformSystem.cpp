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
#define _RX_XX(Klass)											 \
	case BV::Klass:												 \
	{															 \
		Klass##BV& bv = EntityManager::GetComponent<Klass##BV>(handle);\
		bv.RecalculateBV();										 \
		bv.UpdateXform();										 \
		break;													 \
	}

		if (EntityManager::HasComponent<BoundingVolume>(handle))
		{
			auto const& boundingVolume = EntityManager::GetComponent<BoundingVolume>(handle);
			switch (boundingVolume.GetBVType())
			{
				RX_DO_ALL_BV_ENUM_M(_RX_XX)
			default:
				break;
			}
		}
#undef _RX_XX

		//
		if (EntityManager::HasComponent<FrustumBV>(handle))
		{
			FrustumBV& bv = EntityManager::GetComponent<FrustumBV>(handle);

			{
				// 3 points and normal are obvious, 
				// but get the D via normalized normal DOT P0 (any of the points)
				auto MakePlaneEquation =
					[](glm::vec3 const& A, glm::vec3 const& B, glm::vec3 const& C) -> glm::vec4
					{
						glm::vec3 normal = glm::normalize(glm::cross(B - A, C - A));
						float d = glm::dot(normal, C);

						return glm::vec4{ normal, d };
					};

				auto& planeEquations = bv.GetPlaneEquations();
				auto const& fPoints = bv.GetPoints();
#define _RX_XXX(i, A, B, C) planeEquations[i] = MakePlaneEquation(fPoints[A], fPoints[B], fPoints[C])
				_RX_XXX(0, 4, 5, 6);
				_RX_XXX(1, 3, 2, 1);
				_RX_XXX(2, 0, 1, 5);
				_RX_XXX(3, 7, 6, 2);
				_RX_XXX(4, 3, 0, 4);
				_RX_XXX(5, 1, 2, 6);
#undef _RX_XXX
			}
		}
		//

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
			EntityManager::RemoveComponent<Collider::Dirty>(handle);
		}
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
	//{
	//	auto v = EntityManager::View<BoundingVolume::Dirty>();
	//	for (auto [handle, _] : v.each())
	//	{
	//		auto& boundingVolume = EntityManager::GetComponent<BoundingVolume>(handle);
	//		switch (boundingVolume.GetBVType())
	//		{
	//		case BV::AABB:
	//		{
	//			AABBBV& bv = EntityManager::GetComponent<AABBBV>(handle);
	//			bv.RecalculateBV();
	//			bv.UpdateXform();
	//			break;
	//		}
	//		default:
	//			break;
	//		}
	//
	//		EntityManager::RemoveComponent<BoundingVolume::Dirty>(handle);
	//	}
	//}
}
