#include <pch.h>
#include "TransformSystem.h"
#include "ECS/EntityManager.h"
#include "ECS/Components.h"

RX_SINGLETON_EXPLICIT(TransformSystem);

void TransformSystem::Update(float dt)
{
	auto xformView = EntityManager::View<Xform, Xform::Dirty, Metadata>();
	int i = 0;
	for (auto [handle, xform, dirty, meta] : xformView.each())
	{
		xform.UpdateXform();
		EntityManager::RemoveComponent<Xform::Dirty>(handle);
		RX_INFO("{}, {}", i, meta.GetName());
		++i;
	}

	auto colView = EntityManager::View<const Xform, const Collider>();
	for (auto [handle, xform, col] : colView.each())
	{
		if (col.GetPrimitiveType() == Primitive::NIL)
			continue;

		Xform const& xform = EntityManager::GetComponent<const Xform>(handle);
#define _RX_X(Klass)																		\
		case Primitive::Klass:																\
		{																					\
			Klass##Primitive& bv = EntityManager::GetComponent<Klass##Primitive>(handle);	\
			if (bv.IsFollowXform()) bv.GetPosition() = xform.GetTranslate();				\
		} break;

		switch (col.GetPrimitiveType())
		{
			RX_DO_ALL_BV_ENUM;
		default:
			break;
		}
#undef _RX_X

#define _RX_X(Klass)																	\
	case Primitive::Klass:																\
	{																					\
		/*Should check ensure that get<BV> exists*/										\
		Klass##Primitive& bv = EntityManager::GetComponent<Klass##Primitive>(handle);	\
		bv.UpdateXform();																\
	} break;

		switch (col.GetPrimitiveType())
		{
			RX_DO_ALL_BV_ENUM;
		default:
			break;
		}
#undef _RX_X
	}
}
