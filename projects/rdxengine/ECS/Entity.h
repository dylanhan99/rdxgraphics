#ifndef ENTITY_H
#define ENTITY_H
#include "ECS/BaseEntityComponentWorld.h"

namespace rdx
{
	class Entity
	{
	public:
		Entity(BaseEntityComponentWorld* pWorld, EntityID id)
			: m_pWorld(pWorld), m_ID(id)
		{
			RX_ASSERT(m_pWorld);
		}

		template <typename T>
		std::optional<std::reference_wrapper<T>> AddComponent()
		{
			return m_pWorld->AddComponent<T>(m_ID); // May return nullopt if fails for some reason
		}

		template <typename T>
		std::optional<std::reference_wrapper<T>> GetComponent()
		{
			return m_pWorld->GetComponent<T>(m_ID);
		}

		// Entity Clone()
		// bool Destroy()
		// bool IsValid()
		
	private:
		BaseEntityComponentWorld* m_pWorld{ nullptr };
		EntityID m_ID{ RX_INVALID_ENTITY };
	};
}

#endif