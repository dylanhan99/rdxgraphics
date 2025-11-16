#ifndef INSTANTEVENTBUS_H
#define INSTANTEVENTBUS_H
#include <typeindex>

namespace rdx
{
	class InstantEventBus
	{
	public:
		template <typename EventT>
		void Subscribe(std::function<void(EventT const&)> listener)
		{
			auto& list = m_Listeners[typeid(EventT)];
			list.push_back(
				[l = std::move(listener)](void const* e)
				{
					l(*static_cast<const EventT*>(e));
				});
		}

		template <typename EventT>
		void Publish(EventT const& e)
		{
			auto it = m_Listeners.find(typeid(EventT));
			if (it == m_Listeners.end())
				return;

			for (auto& callback : it->second)
				callback(&e);
		}

	private:
		using Callback = std::function<void(void const*)>;
		std::unordered_map<std::type_index, std::vector<Callback>> m_Listeners{};
	};
}

#endif