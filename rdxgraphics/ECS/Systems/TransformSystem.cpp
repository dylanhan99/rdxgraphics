#include <pch.h>
#include "TransformSystem.h"
#include "ECS/EntityManager.h"
#include "ECS/Components.h"

RX_SINGLETON_EXPLICIT(TransformSystem);

void TransformSystem::Update(float dt)
{
	auto xformView = EntityManager::View<Xform>();
	for (auto [handle, xform] : xformView.each())
	{
		xform.UpdateXform();
	}

	auto colView = EntityManager::View<const Xform, const Collider>();
	for (auto [handle, xform, col] : colView.each())
	{
		if (col.GetPrimitiveType() == Primitive::NIL)
			continue;

		Xform& xform = EntityManager::GetComponent<Xform>(handle);
#define _RX_X(Klass)														 \
		case Primitive::Klass:														 \
		{																	 \
			Klass##Primitive& bv = EntityManager::GetComponent<Klass##Primitive>(handle);  \
			if (bv.IsFollowXform()) bv.GetPosition() = xform.GetTranslate(); \
		} break;

		switch (col.GetPrimitiveType())
		{
			RX_DO_ALL_BV_ENUM;
		default:
			break;
		}
#undef _RX_X

#define _RX_X(Klass)													\
	case Primitive::Klass:														\
	{																	\
		/*Should check ensure that get<BV> exists*/						\
		Klass##Primitive& bv = EntityManager::GetComponent<Klass##Primitive>(handle);	\
		bv.UpdateXform();												\
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
