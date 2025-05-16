#include <pch.h>
#include "SceneManager.h"
#include "ECS/EntityManager.h"

RX_SINGLETON_EXPLICIT(SceneManager);

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
		EntityManager::Clear();
		return false;
	}
	else if (IsRestart(curr))
	{
		g.m_WorkingScene->Start();
	}
	else
	{
		if (g.m_WorkingScene)
		{
			g.m_WorkingScene->Unload();
			EntityManager::Clear();
		}
		g.m_WorkingScene = g.m_Scenes[curr];
		RX_ASSERT(g.m_WorkingScene);
		g.m_WorkingScene->Load();
		g.m_WorkingScene->Start();
	}

	return true;
}
