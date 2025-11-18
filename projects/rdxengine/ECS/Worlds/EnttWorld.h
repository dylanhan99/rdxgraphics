#ifndef ENTTWORLD_H
#define ENTTWORLD_H
#include "ECS/BaseEntityComponentWorld.h"
#include <entt/entt.hpp>

namespace rdx
{
	class EnttWorld final : public BaseEntityComponentWorldT<EnttWorld>
	{
	public:
		~EnttWorld() override;

		bool InitWorld() override;
		bool TerminateImpl() override;

		Entity CreateEntity() override;
		bool HasEntity(EntityID const) override;

		template <typename T>
		bool const HasComponentImpl(EntityID const eid)
		{
			RX_ASSERT(HasEntity(eid));

			entt::entity enttid = m_HandleMap.at(eid);
			return m_Registry.all_of<T>(enttid);
		}

		template <typename T>
		void* const AddComponentImpl(EntityID const eid)
		{
			RX_ASSERT(HasEntity(eid));

			entt::entity enttid = m_HandleMap.at(eid);
			return &m_Registry.emplace_or_replace<T>(enttid);
		}

		template <typename T>
		void* const GetComponentImpl(EntityID const eid)
		{
			RX_ASSERT(HasEntity(eid));

			entt::entity enttid = m_HandleMap.at(eid);
			return m_Registry.try_get<T>(enttid);
		}

	private:
		void MapHandle(EntityID const, entt::entity const);

	private:
		entt::registry m_Registry{};
		std::map<EntityID, entt::entity> m_HandleMap{};
	};
}

#endif