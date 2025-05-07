#pragma once
#include "Entity.h"

class EntityManager : public BaseSingleton<EntityManager>
{
	RX_SINGLETON_DECLARATION(EntityManager);
public:
	static bool Init();
	static void Terminate();

	inline static std::vector<Entity>& GetEntities() { return g.m_Entities; }

private:
	std::vector<Entity> m_Entities{};
};