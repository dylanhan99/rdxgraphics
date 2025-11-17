#ifndef COMPONENTBASEDWORLD_H
#define COMPONENTBASEDWORLD_H
#include "ECS/BaseEntityComponentWorld.h"

namespace rdx
{
	class ComponentBasedWorld final : public BaseEntityComponentWorldT<ComponentBasedWorld>
	{
	public:
		~ComponentBasedWorld() override;

		bool InitWorld() override;
		bool TerminateImpl() override;

		Entity CreateEntity() override;
		bool HasEntity(EntityID const) override;

		template <typename T>
		bool const HasComponentImpl(EntityID const eid)
		{
			return false;
		}

		template <typename T>
		void* const AddComponentImpl(EntityID const eid)
		{
			return nullptr;
		}

		template <typename T>
		void* const GetComponentImpl(EntityID const eid)
		{
			return nullptr;
		}
	};
}

#endif