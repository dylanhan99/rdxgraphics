#include "ComponentBasedWorld.h"
#include "ECS/Entity.h"

using namespace rdx;

ComponentBasedWorld::~ComponentBasedWorld()
{

}

bool ComponentBasedWorld::InitWorld()
{
	return true;
}

bool ComponentBasedWorld::TerminateImpl()
{
	return true;
}

Entity ComponentBasedWorld::CreateEntity()
{
	return Entity{ this, 0 };
}

bool ComponentBasedWorld::HasEntity(EntityID const eid)
{
	return false;
}
