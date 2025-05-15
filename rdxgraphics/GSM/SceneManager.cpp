#include <pch.h>
#include "SceneManager.h"
#include "Scenes/Sandbox.h"

RX_SINGLETON_EXPLICIT(SceneManager);

void SceneManager::Init()
{
	RegisterScene<Sandbox>();

	RX_ASSERT(!g.m_Scenes.empty(), "No scenes?");

	GetNextScene() = 0; // Arbitrarily start on the first.
}

void SceneManager::Terminate()
{
	for (auto& pScene : g.m_Scenes)
		pScene->Unload();
	g.m_Scenes.clear();
}

bool SceneManager::ResolveScenes()
{
	if (GetCurrScene() == GetNextScene())
		return true;

	GetPrevScene() = GetCurrScene();
	GetCurrScene() = GetNextScene();

	size_t const& curr = GetCurrScene();
	if (IsQuit(curr))
	{
		g.m_WorkingScene->Unload();
		return false;
	}
	else if (IsRestart(curr))
	{
		g.m_WorkingScene->Start();
	}
	else
	{
		if (g.m_WorkingScene)
			g.m_WorkingScene->Unload();
		g.m_WorkingScene = g.m_Scenes[curr];
		RX_ASSERT(g.m_WorkingScene);
		g.m_WorkingScene->Load();
		g.m_WorkingScene->Start();
	}

	return true;
}
