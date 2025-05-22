#pragma once
#include "BaseScene.h"

// GSM. But i don't like acronyms as class names, and GameStateManager is too long
class SceneManager : public BaseSingleton<SceneManager>
{
	RX_SINGLETON_DECLARATION(SceneManager);
private:
	enum class State { // States are used as offsets also
		Restart,
		Quit
	};

public:
	// T > The CommonScene
	template <typename T>
	static void Init()
	{
		g.m_CommonScene = std::make_shared<T>("Common", 
				"Engine specific controls to prevent intrusion into engine processes");
		GetNextScene() = 0; // Arbitrarily start on the first.
		RX_ASSERT(!g.m_Scenes.empty(), "No scenes?");
		RX_ASSERT(g.m_CommonScene, "Missing common scene");
	}
	static void Terminate();
	static bool ResolveScenes();
	static void Update(float dt);

	template <typename T, typename ...Args>
	static std::enable_if_t<std::is_base_of_v<BaseScene, T> && std::is_constructible_v<T, Args...>,
		void> RegisterScene(Args&& ...args)
	{
		g.m_Scenes.emplace_back(std::make_shared<T>(std::forward<Args>(args)...));
	}

	inline static size_t& GetPrevScene() { return g.m_PrevScene; }
	inline static size_t& GetCurrScene() { return g.m_CurrScene; }
	inline static size_t& GetNextScene() { return g.m_NextScene; }

	inline static void SetNextScene(size_t s) { g.m_NextScene = s; }

	inline static void Restart() { g.m_NextScene = g.m_Scenes.size() + (size_t)State::Restart; }
	inline static void Quit() { g.m_NextScene = g.m_Scenes.size() + (size_t)State::Quit; }

	inline static bool IsRestart(size_t s) { return s == g.m_Scenes.size() + (size_t)State::Restart; }
	inline static bool IsQuit(size_t s) { return s == g.m_Scenes.size() + (size_t)State::Quit; }

	inline static std::shared_ptr<BaseScene> GetCommonScene() { return g.m_CommonScene; }
	inline static std::shared_ptr<BaseScene> GetWorkingScene() { return g.m_WorkingScene; }

private:
	std::vector<std::shared_ptr<BaseScene>> m_Scenes{};

	size_t m_PrevScene{ std::numeric_limits<size_t>::max() };
	size_t m_CurrScene{ std::numeric_limits<size_t>::max() };
	size_t m_NextScene{ std::numeric_limits<size_t>::max() };

	// This serves as a method of having "scriptable" common logic
	// Think of this scene as "engine scene" which provides engine
	// specific logic without being intrusive to actual engine processes,
	// while WorkingScene is the "game scene" as we understand it.
	std::shared_ptr<BaseScene> m_CommonScene{ nullptr };

	// Actual scene that will change etc.
	std::shared_ptr<BaseScene> m_WorkingScene{ nullptr };
};