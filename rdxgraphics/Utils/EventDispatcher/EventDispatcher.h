#pragma once

template <typename ...Args>
class EventDispatcher : public BaseSingleton<EventDispatcher<Args...>>
{
	RX_SINGLETON_DECLARATION(EventDispatcher<Args...>);
private:
	using EventName = std::string;
	using PFNOnEvent = std::function<void(Args...)>;
	using EventListeners = std::vector<PFNOnEvent>;
	using RegisteredEvents = std::unordered_map<std::string, EventListeners>;

public:
	// Register a function with fixed arguments to an event name.
	static void RegisterEvent(EventName const& eventName, PFNOnEvent onEvent);

	// Fire all PFNOnEvent which correspond to eventName
	static void FireEvent(EventName const& eventName, Args... args);

private:
	RegisteredEvents m_RegisteredEvents{ 1 }; // unique to each EventDispatcher template
};


template <typename ...Args>
void EventDispatcher<Args...>::RegisterEvent(EventName const& eventName, PFNOnEvent onEvent)
{
	g.m_RegisteredEvents[eventName].emplace_back(std::move(onEvent));
}

template <typename ...Args>
void EventDispatcher<Args...>::FireEvent(EventName const& eventName, Args... args)
{
	auto it = g.m_RegisteredEvents.find(eventName);
	if (it == g.m_RegisteredEvents.end())
		return;
	EventListeners& listeners{ it->second };
	for (PFNOnEvent& pfn : listeners)
	{
//#pragma warning(suppress: 26800)
		pfn(std::forward<Args>(args)...);
	}
}