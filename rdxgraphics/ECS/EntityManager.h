#pragma once
#include "Entity.h"
#include <entt/entt.hpp>
	
// Helper to check that component has the HasEnttHandle boolean
template <typename, typename = void>
struct has_entt_handle : std::false_type {};
template <typename T>
struct has_entt_handle<T, std::void_t<decltype(T::HasEnttHandle)>> : std::true_type {};
template<typename T>
constexpr bool has_entt_handle_v = has_entt_handle<T>::value;

class EntityManager : public BaseSingleton<EntityManager>
{
	RX_SINGLETON_DECLARATION(EntityManager);
public:
	static bool Init();
	static void Terminate();

	template <typename ...Args>
	static std::enable_if_t<(std::is_base_of_v<BaseComponent, Args> && ...),  // could use conjunction, but this is more legible
		entt::entity> CreateEntity()
	{
		entt::entity handle = g.m_Registry.create();
		(AddComponent<Args>(handle), ...); // Default ctor each type provided
		return handle;
	}

	template <typename T, typename ...Args>
	static std::enable_if_t<
		std::is_base_of_v<BaseComponent, T> &&
		(has_entt_handle_v<T> ? 
			std::is_constructible_v<T, entt::entity, Args...> :
			std::is_constructible_v<T, Args...>),
		void> AddComponent(entt::entity handle, Args&& ...args)
	{
		if constexpr (has_entt_handle<T>::value)
		{
			entt::entity copyHandle = handle;
			g.m_Registry.emplace_or_replace<T, entt::entity, Args...>(handle, std::move(copyHandle), std::forward<Args>(args)...);
		}
		else
		{
			g.m_Registry.emplace_or_replace<T, Args...>(handle, std::forward<Args>(args)...);
		}
	}

	template <typename T>
	static std::enable_if_t<std::is_base_of_v<BaseComponent, T>,
		void> RemoveComponent(entt::entity handle)
	{
		g.m_Registry.remove<T>(handle);
	}

	//template <typename ...Args>
	//static auto View()
	//{
	//	return m_Registry.view<Args...>();
	//}

	//inline static std::vector<Entity>& GetEntities() { return g.m_Entities; }

public:
	//std::vector<Entity> m_Entities{};

	entt::registry m_Registry{};
};