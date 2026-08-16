#pragma once

//temp
struct FrameBufferResize
{
    int x,y;
};

enum class EventPhase
{
    PreRender,

    MAX
};

class EventBus : public BaseSingleton<EventBus>
{
    RX_SINGLETON_DECLARATION(EventBus);
private:
    struct IQueue
    {
        virtual ~IQueue() = default;
        virtual void Flush() = 0;
    };

public:
    template <typename T>
    using Callback = std::function<void(T const&)>;
    template <typename T>
    struct TypedQueue : IQueue
    {
        void Subscribe(Callback<T> cb) { Callbacks.emplace_back(cb); }
        void Raise(T e) { Events.emplace_back(e); }
        void Flush() override;

        std::vector<Callback<T>> Callbacks; // The vector type could be replaced with custom type so it carries priority
        std::vector<T> Events; // Technically, many events might be duplicated. But events are supposed to be relatively lightweight, so I'll take that memory cost for implementation simplicity
    };
    
public:
    template <typename T>
    static void Subscribe(EventPhase phase, Callback<T> cb);
    template <typename T>
    static void Raise(T e);
    static void Dispatch(EventPhase phase);

private:
    template <typename T>
    static std::unique_ptr<IQueue>& GetQueue(EventPhase phase);

private:
    std::map<EventPhase, 
        std::unordered_map< // Map the type to the callbacks and queue of unprocessed events
            std::type_index,
            std::unique_ptr<IQueue> //std::vector<std::function<void(T)> // This can be a struct or something. Adding further priority/sorting for the functions.
        >> m_EventMap{};
};

template <typename T>
void EventBus::Subscribe(EventPhase phase, Callback<T> cb)
{
    RX_ASSERT(phase < EventPhase::MAX);
    auto& typedQueue = GetQueue<T>(phase);
    typedQueue->Subscribe(cb);
}

template <typename T>
void EventBus::Raise(T e)
{
    for (auto& [phase, phaseMap] : g.m_EventMap)
    {
        auto& typedQueue = GetQueue<T>(phase);
        typedQueue.Raise(e);
    }
}

template <typename T>
std::unique_ptr<EventBus::IQueue>& EventBus::GetQueue(EventPhase phase)
{
    auto& phaseMap = g.m_EventMap[phase];
    auto id = std::type_index(typeid(T));
    auto& uptr = phaseMap[id]; 

    if (!uptr) 
        uptr = std::make_unique<TypedQueue<T>>(new TypedQueue<T>{});

    return uptr;
}

template <typename T>
void EventBus::TypedQueue<T>::Flush()
{
    for (T const& e : Events)
        for (auto const& cb : Callbacks)
            cb(e);

    Events.clear();
}