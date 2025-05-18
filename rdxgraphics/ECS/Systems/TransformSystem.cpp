#include <pch.h>
#include "TransformSystem.h"
#include "ECS/EntityManager.h"
#include "ECS/Components/Camera.h"

RX_SINGLETON_EXPLICIT(TransformSystem);

void TransformSystem::Update(float dt)
{
	auto camView = EntityManager::View<Camera>();
	for (auto [handle, cam] : camView.each())
	{
		cam.UpdateCameraVectors();
	}

	auto xformView = EntityManager::View<Xform>();
	for (auto [handle, xform] : xformView.each())
	{
		xform.UpdateXform();
	}

	auto colView = EntityManager::View<const Xform, const Collider>();
	for (auto [handle, xform, col] : colView.each())
	{
		if (col.GetBVType() == BV::NIL)
			continue;

		Xform& xform = EntityManager::GetComponent<Xform>(handle);
#define _RX_X(Klass)														 \
		case BV::Klass:														 \
		{																	 \
			Klass##BV& bv = EntityManager::GetComponent<Klass##BV>(handle);  \
			if (bv.IsFollowXform()) bv.GetPosition() = xform.GetTranslate(); \
		} break;

		switch (col.GetBVType())
		{
			RX_DO_ALL_BV_ENUM;
		default:
			break;
		}
#undef _RX_X

#define _RX_X(Klass)													\
	case BV::Klass:														\
	{																	\
		/*Should check ensure that get<BV> exists*/						\
		Klass##BV& bv = EntityManager::GetComponent<Klass##BV>(handle);	\
		bv.UpdateXform();												\
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
