#pragma once
#include "Components/BaseComponent.h"

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

	// Ensure that the components have copy ctor enabled.
	static entt::entity CloneEntity(entt::entity handle);

	template <typename T, typename ...Args>
	static std::enable_if_t<
		true,
		//std::is_base_of_v<BaseComponent, T> &&
		//std::is_constructible_v<T, Args...>,
		T&> AddComponent(entt::entity handle, Args&& ...args)
	{
		return g.m_Registry.emplace_or_replace<T, Args...>(handle, std::forward<Args>(args)...);
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

	template <typename ...Args>
	static void Clear()
	{
		g.m_Registry.clear<Args...>();
	}

	inline static void Destroy(entt::entity handle) { g.m_Registry.destroy(handle); }
	inline static void Destroy(std::initializer_list<entt::entity>& handles) { for (auto h : handles) Destroy(h); }
	inline static void Destroy(std::vector<entt::entity>& handles) { for (auto h : handles) Destroy(h); handles.clear(); }

	static bool HasEntity(entt::entity handle);

public:
	inline static entt::registry m_Registry{};
};