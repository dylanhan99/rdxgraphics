#include "EventBus.h"

RX_SINGLETON_DEFINITION(EventBus)

void EventBus::Dispatch()
{
    for (auto& phaseMap : g.m_EventMap)
        phaseMap->Flush();
}