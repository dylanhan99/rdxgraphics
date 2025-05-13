#pragma once
#include <entt/entt.hpp>
#include "Components/BaseComponent.h"
	
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
		T&> AddComponent(entt::entity handle, Args&& ...args)
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

		return GetComponent<T>(handle);
	}

	template <typename T>
	static std::enable_if_t<std::is_base_of_v<BaseComponent, T>,
		void> RemoveComponent(entt::entity handle)
	{
		g.m_Registry.remove<T>(handle);
	}

	template <typename ...Args>
	static std::enable_if_t<(std::is_base_of_v<BaseComponent, Args> && ...),
		bool> HasComponent(entt::entity handle)
	{
		return g.m_Registry.all_of<Args...>(handle);
	}

	template <typename T>
	static auto& GetComponent(entt::entity handle)
	{
		// Need to static assert instead of sfinae cus i can't enable_if_t and use auto as return
		static_assert(std::is_base_of_v<BaseComponent, T>);
		RX_ASSERT(HasComponent<T>(handle), R"(Check HasComponent<T> before GetComponent.)");

		return g.m_Registry.get<T>(handle);
	}

	template <typename T1, typename T2, typename ...Args>
	static auto GetComponent(entt::entity handle)
	{
		// Need to static assert instead of sfinae cus i can't enable_if_t and use auto as return
		static_assert(std::conjunction_v<
			std::is_base_of<BaseComponent, T1>, 
			std::is_base_of<BaseComponent, T2>, 
			std::is_base_of<BaseComponent, Args>...>);
		RX_ASSERT((HasComponent<T1, T2, Args...>(handle)), R"(Check HasComponent<T> before GetComponent.)");

		return g.m_Registry.get<T1, T2, Args...>(handle);
	}

	template <typename T, typename ...Other, typename ...Exclude>
	static auto View(entt::exclude_t<Exclude...> exc = entt::exclude_t{})
	{
		return g.m_Registry.view<T, Other...>(exc);
	}

	static bool HasEntity(entt::entity handle);

public:
	inline static entt::registry m_Registry{};
};