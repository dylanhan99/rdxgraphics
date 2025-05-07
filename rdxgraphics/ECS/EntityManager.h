#pragma once
#include "Entity.h"
#include <entt/entt.hpp>

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
		AddComponent<Args>(handle), ...; // Default ctor each type provided
		return handle;
	}

	template <typename T, typename ...Args>
	static std::enable_if_t<
		std::is_base_of_v<BaseComponent, T> &&
		std::is_constructible_v<T, Args...>,
		void> AddComponent(entt::entity handle, Args&& ...args)
	{
		g.m_Registry.emplace_or_replace<T, Args...>(handle, std::forward<Args>(args)...);
	}

	template <typename T>
	static std::enable_if_t<std::is_base_of_v<BaseComponent, T>,
		void> RemoveComponent(entt::entity handle)
	{
		g.m_Registry.remove<T>(handle)
	}

	inline static std::vector<Entity>& GetEntities() { return g.m_Entities; }

private:
	std::vector<Entity> m_Entities{};

	entt::registry m_Registry{};
};