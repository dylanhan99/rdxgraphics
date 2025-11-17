#include "EnttWorld.h"
#include "ECS/Entity.h"

using namespace rdx;

EnttWorld::~EnttWorld()
{

}

bool EnttWorld::InitWorld()
{
	return true;
}

bool EnttWorld::TerminateImpl()
{
	m_Registry.clear();
	m_HandleMap.clear();

	return true;
}

Entity EnttWorld::CreateEntity()
{
	EntityID eid = GenerateEntityID();
	entt::entity enttid = m_Registry.create();
	MapHandle(eid, enttid);

	return Entity{ this, eid };
}

bool EnttWorld::HasEntity(EntityID const eid)
{
	return m_HandleMap.find(eid) != m_HandleMap.end();
}

void EnttWorld::MapHandle(EntityID const eid, entt::entity const enttid)
{
	m_HandleMap[eid] = enttid;
}