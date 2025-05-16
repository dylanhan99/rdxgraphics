#include <pch.h>
#include "SceneManager.h"
#include "ECS/EntityManager.h"

RX_SINGLETON_EXPLICIT(SceneManager);

void SceneManager::Terminate()
{
	g.m_CommonScene.reset();
	g.m_WorkingScene.reset();
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
		g.m_CommonScene->Unload();
		if (g.m_WorkingScene)g.m_WorkingScene->Unload();
		EntityManager::Clear();
		return false;
	}
	else if (IsRestart(curr))
	{
		g.m_CommonScene->Start();
		if (g.m_WorkingScene) g.m_WorkingScene->Start();
	}
	else
	{
		g.m_CommonScene->Start();
		if (g.m_WorkingScene)
		{
			g.m_WorkingScene->Unload();
			EntityManager::Destroy(g.m_WorkingScene->GetEntities());
		}
		g.m_WorkingScene = g.m_Scenes[curr];
		RX_ASSERT(g.m_WorkingScene);
		g.m_WorkingScene->Load();
		g.m_WorkingScene->Start();
	}

	return true;
}

void SceneManager::Update(float dt)
{
	g.m_CommonScene->Update(dt);
	if (g.m_WorkingScene) g.m_WorkingScene->Update(dt);
}
