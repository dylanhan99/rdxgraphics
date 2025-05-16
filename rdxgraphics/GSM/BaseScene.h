#pragma once
#include "ECS/EntityManager.h"
#include "ECS/Components.h"
#define RX_SCENE_DEFAULT(Foo) public: inline Foo(std::string const& name, std::string const& desc = "") : BaseScene(name, desc) {} private:

class BaseScene
{
public:
	inline BaseScene(std::string const& name, std::string const& desc) 
		: m_SceneName(name), m_SceneDesc(desc) {}
	~BaseScene() = default;

	virtual void Load() = 0;
	inline virtual void UnloadImpl(){};
	virtual void Start() = 0;
	virtual void Update(float dt) = 0;

	template <typename ...Args>
	entt::entity CreateEntity()
	{
		entt::entity handle = EntityManager::CreateEntity<NoDelete, Args...>();
		m_Handles.push_back(handle);
		return handle;
	}

	inline void Unload()
	{
		UnloadImpl();
		EntityManager::Destroy(m_Handles);
	}

	inline std::string const& GetSceneName() const { return m_SceneName; }
	inline std::string const& GetSceneDesc() const { return m_SceneDesc; }
	inline std::vector<entt::entity> const& GetEntities() const { return m_Handles; }
	inline std::vector<entt::entity>& GetEntities() { return m_Handles; }

private:
	std::string m_SceneName{};
	std::string m_SceneDesc{};

	std::vector<entt::entity> m_Handles{};
};