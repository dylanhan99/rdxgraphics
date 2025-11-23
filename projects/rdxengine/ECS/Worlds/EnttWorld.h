#ifndef ENTTWORLD_H
#define ENTTWORLD_H
#include "ECS/BaseEntityComponentWorld.h"
#include <entt/entt.hpp>
#include "ServiceLayer.h"

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

			entt::entity enttid = m_EidEnttMap.at(eid);
			return m_Registry.all_of<T>(enttid);
		}

		template <typename T>
		void* const AddComponentImpl(EntityID const eid)
		{
			RX_ASSERT(HasEntity(eid));

			entt::entity enttid = m_EidEnttMap.at(eid);
			return &m_Registry.emplace_or_replace<T>(enttid);
		}

		template <typename T>
		void* const GetComponentImpl(EntityID const eid)
		{
			RX_ASSERT(HasEntity(eid));

			entt::entity enttid = m_EidEnttMap.at(eid);
			return m_Registry.try_get<T>(enttid);
		}

		template <typename C, typename ...Cs>
		void ViewImpl(ViewEachFn<C, Cs...> fnEach)
		{
			auto view = m_Registry.view<C, Cs...>();
			for (auto tup : view.each())
			{
				std::apply([this, &fnEach](auto enttid, auto& ...comps) {
					fnEach(GetMapHandle(enttid), comps...); 
				}, tup);
			}
		}

		void ClearImpl() override;

	private:
		entt::entity GetMapHandle(EntityID const);
		EntityID GetMapHandle(entt::entity const);
		void SetMapHandle(EntityID const, entt::entity const);

	private:
		entt::registry m_Registry{};
		std::map<EntityID, entt::entity> m_EidEnttMap{};
		std::map<entt::entity, EntityID> m_EnttEidMap{};
	};
}

#endif