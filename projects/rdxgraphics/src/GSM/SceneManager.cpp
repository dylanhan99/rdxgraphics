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
		if (g.m_CommonScene)  g.m_CommonScene->Stop();
		if (g.m_WorkingScene) g.m_WorkingScene->Stop();
		EntityManager::Clear();
		return false;
	}
	else if (IsRestart(curr))
	{
		if (g.m_CommonScene)  g.m_CommonScene->Stop();
		if (g.m_WorkingScene) g.m_WorkingScene->Stop();

		SetNextScene(GetPrevScene());
	}
	else
	{
		if (g.m_CommonScene)  g.m_CommonScene->Stop();
		if (g.m_WorkingScene) g.m_WorkingScene->Stop();

		g.m_WorkingScene = g.m_Scenes[curr];

		if (g.m_CommonScene)  g.m_CommonScene->Start();
		if (g.m_WorkingScene) g.m_WorkingScene->Start();
	}

	return true;
}

void SceneManager::Update(float dt)
{
	if (g.m_CommonScene)  g.m_CommonScene->Update(dt);
	if (g.m_WorkingScene) g.m_WorkingScene->Update(dt);
}
