#pragma once
#include "Events.h"

namespace EventBusDetail
{ // SFINAE magic
    template <typename F>
    struct CallableArgType : CallableArgType<decltype(&std::decay_t<F>::operator())> {};

    template <typename C, typename Ret, typename Arg>
    struct CallableArgType<Ret(C::*)(Arg) const>
    {
        using Type = std::decay_t<Arg>;
    };

    template <typename C, typename Ret, typename Arg>
    struct CallableArgType<Ret(C::*)(Arg)>
    {
        using Type = std::decay_t<Arg>;
    };

    template <typename Ret, typename Arg>
    struct CallableArgType<Ret(*)(Arg)>
    {
        using Type = std::decay_t<Arg>;
    };

    template <typename Ret, typename Arg>
    struct CallableArgType<Ret(Arg)>
    {
        using Type = std::decay_t<Arg>;
    };

    // Owns raw events of type T for a given phase. Callback invocation
    // is handled separately by ISubscription, NOT here.
    struct IQueue
    {
        virtual ~IQueue() = default;
        virtual void Flush() = 0; // clears events, does NOT invoke callbacks
    };

    // Type-erased handle so subscriptions of different event types can be
    // stored together in one list and sorted by priority.
    struct ISubscription
    {
        virtual ~ISubscription() = default;
        virtual void Invoke() = 0;
        int Priority = 0;
    };
}

class EventBus : public BaseSingleton<EventBus>
{
    RX_SINGLETON_DECLARATION(EventBus);
public:
    template <typename T>
    using Callback = std::function<void(T const&)>;

    template <typename T>
    struct TypedQueue : EventBusDetail::IQueue
    {
        void Raise(T e) { Events.emplace_back(std::move(e)); }
        void Flush() override { Events.clear(); }

        std::vector<T> Events; // Technically, many events might be duplicated. But events are supposed to be relatively lightweight, so I'll take that memory cost for implementation simplicity
    };

    template <typename T>
    struct TypedSubscription : EventBusDetail::ISubscription
    {
        Callback<T> Cb;
        TypedQueue<T>* Queue = nullptr;

        void Invoke() override
        {
            for (T const& e : Queue->Events)
                Cb(e);
        }
    };

public:
    // Lower priority values run first
    template <typename F>
    static void Subscribe(EventPhase phase, F&& cb, int priority = 0);
    template <typename T>
    static void Raise(T e);
    static void Dispatch(EventPhase phase);

private:
    template <typename T>
    static void SubscribeTyped(EventPhase phase, Callback<T> cb, int priority);
    template <typename T>
    static EventBus::TypedQueue<T>& GetQueue(EventPhase phase);

private:
    // Per-phase, per-type storage of raised events (no callbacks here)
    std::map<EventPhase,
        std::unordered_map<
            std::type_index,
            std::unique_ptr<EventBusDetail::IQueue>
        >
    > m_EventMap{};

    // Per-phase, priority-sorted list of subscriptions across ALL event types.
    // This guarantees global ordering within a phase.
    std::map<EventPhase,
        std::vector<std::unique_ptr<EventBusDetail::ISubscription>>
    > m_Subscriptions{};
};

template <typename F>
void EventBus::Subscribe(EventPhase phase, F&& cb, int priority)
{
    using T = typename EventBusDetail::CallableArgType<std::decay_t<F>>::Type;
    SubscribeTyped<T>(phase, Callback<T>(std::forward<F>(cb)), priority);
}

template <typename T>
void EventBus::Raise(T e)
{
    for (auto& [phase, phaseMap] : g.m_EventMap)
        GetQueue<T>(phase).Raise(e);
}

template <typename T>
void EventBus::SubscribeTyped(EventPhase phase, Callback<T> cb, int priority)
{
    RX_ASSERT(phase < EventPhase::MAX);

    auto sub = std::make_unique<TypedSubscription<T>>();
    sub->Cb = std::move(cb);
    sub->Queue = &GetQueue<T>(phase);
    sub->Priority = priority;

    auto& list = g.m_Subscriptions[phase];
    auto it = std::upper_bound(list.begin(), list.end(), priority,
        [](int p, auto const& existing) { return p < existing->Priority; });
    list.insert(it, std::move(sub));
}

template <typename T>
EventBus::TypedQueue<T>& EventBus::GetQueue(EventPhase phase)
{
    auto& phaseMap = g.m_EventMap[phase];
    auto id = std::type_index(typeid(T));
    auto& uptr = phaseMap[id];

    if (!uptr)
        uptr = std::make_unique<TypedQueue<T>>();

    return *static_cast<TypedQueue<T>*>(uptr.get());
}

void EventBus::Dispatch(EventPhase phase)
{
    RX_ASSERT(phase < EventPhase::MAX);

    // Invoke callbacks in globally sorted priority order, across all event types.
    for (auto& sub : g.m_Subscriptions[phase])
        sub->Invoke();

    // Clear all events for this phase now that every subscriber has seen them.
    for (auto& [id, queue] : g.m_EventMap[phase])
        queue->Flush();
}