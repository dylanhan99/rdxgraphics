#pragma once

// This really just is a glorified container for explicitly setting up call orders where possible
class EventManager : public BaseSingleton<EventManager>
{
	RX_SINGLETON_DECLARATION(EventManager);
public:
	void Init();
};