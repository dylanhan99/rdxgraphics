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
	static void Init();
	static void Terminate();
	static bool ResolveScenes();

	template <typename T>
	static std::enable_if_t<std::is_base_of_v<BaseScene, T>,
		void> RegisterScene()
	{
		g.m_Scenes.emplace_back(std::make_shared<T>());
	}

	inline static size_t& GetPrevScene() { return g.m_PrevScene; }
	inline static size_t& GetCurrScene() { return g.m_CurrScene; }
	inline static size_t& GetNextScene() { return g.m_NextScene; }

	inline static void Restart() { g.m_NextScene = g.m_Scenes.size() + (size_t)State::Restart; }
	inline static void Quit() { g.m_NextScene = g.m_Scenes.size() + (size_t)State::Quit; }

	inline static bool IsRestart(size_t s) { return s == g.m_Scenes.size() + (size_t)State::Restart; }
	inline static bool IsQuit(size_t s) { return s == g.m_Scenes.size() + (size_t)State::Quit; }

	inline static std::shared_ptr<BaseScene> GetWorkingScene() { return g.m_WorkingScene; }

private:
	std::vector<std::shared_ptr<BaseScene>> m_Scenes{};

	size_t m_PrevScene{ std::numeric_limits<size_t>::max() };
	size_t m_CurrScene{ std::numeric_limits<size_t>::max() };
	size_t m_NextScene{ std::numeric_limits<size_t>::max() };

	std::shared_ptr<BaseScene> m_WorkingScene{ nullptr };
};