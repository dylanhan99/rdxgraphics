#pragma once
#include "ECS/EntityManager.h"
#include "ECS/Components.h"
#include "ECS/Systems/CollisionSystem.h"
#define RX_SCENE_DEFAULT(Foo) public: inline Foo(std::string const& name, std::string const& desc = "") : BaseScene(name, desc) {} private:

class BaseScene
{
public:
	inline BaseScene(std::string const& name, std::string const& desc) 
		: m_SceneName(name), m_SceneDesc(desc) {}
	~BaseScene() = default;

	inline virtual void StartImpl() {};
	inline virtual void UpdateImpl(float dt) {};
	inline virtual void StopImpl() {};

	template <typename ...Args>
	inline entt::entity CreateDefaultEntity()
	{
		entt::entity handle = CreateEntity<Metadata, Xform, Args...>();
		EntityManager::AddComponent<BV>(handle, CollisionSystem::GetGlobalBVType());
		return handle;
	}

	inline void DestroyEntity(entt::entity handle)
	{
		EntityManager::Destroy(handle);
		std::erase(m_Handles, handle);
	}

	inline entt::entity CloneEntity(entt::entity handle)
	{
		entt::entity clone = EntityManager::CloneEntity(handle);
		m_Handles.push_back(clone);
		return clone;
	}

	inline void Start()
	{
		StartImpl();
	}

	inline void Update(float dt)
	{
		UpdateImpl(dt);
	}

	inline void Stop()
	{
		StopImpl();
		EntityManager::Destroy(m_Handles);
	}

	inline std::string const& GetSceneName() const { return m_SceneName; }
	inline std::string const& GetSceneDesc() const { return m_SceneDesc; }
	inline std::vector<entt::entity> const& GetEntities() const { return m_Handles; }
	inline std::vector<entt::entity>& GetEntities() { return m_Handles; }

private:
	template <typename ...Args>
	entt::entity CreateEntity()
	{
		entt::entity handle = EntityManager::CreateEntity<Args...>();
		m_Handles.push_back(handle);
		return handle;
	}

private:
	std::string m_SceneName{};
	std::string m_SceneDesc{};

	std::vector<entt::entity> m_Handles{};
};