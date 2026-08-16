#include "EventBus.h"

RX_SINGLETON_EXPLICIT(EventBus)

void EventBus::Dispatch(EventPhase phase)
{
    for (auto& [id, queue] : g.m_EventMap[phase])
        queue->Flush();
}