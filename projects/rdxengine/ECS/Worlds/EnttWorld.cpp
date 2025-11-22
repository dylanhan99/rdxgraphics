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
	m_EidEnttMap.clear();
	m_EnttEidMap.clear();

	return true;
}

Entity EnttWorld::CreateEntity()
{
	EntityID eid = GenerateEntityID();
	entt::entity enttid = m_Registry.create();
	SetMapHandle(eid, enttid);

	return Entity{ this, eid };
}

bool EnttWorld::HasEntity(EntityID const eid)
{
	return m_EidEnttMap.find(eid) != m_EidEnttMap.end();
}

entt::entity EnttWorld::GetMapHandle(EntityID const eid)
{
	auto it = m_EidEnttMap.find(eid);
	return it != m_EidEnttMap.end() ? it->second : entt::null;
}

EntityID EnttWorld::GetMapHandle(entt::entity const enttid)
{
	auto it = m_EnttEidMap.find(enttid);
	return it != m_EnttEidMap.end() ? it->second : RX_INVALID_ENTITY;
}

void EnttWorld::SetMapHandle(EntityID const eid, entt::entity const enttid)
{
	m_EidEnttMap[eid] = enttid;
	m_EnttEidMap[enttid] = eid;
}