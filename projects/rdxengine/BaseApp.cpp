#include "BaseSystem.h"
#include "BaseUtil.h"
#include "Profiling/PerformanceProfiler.h"

using namespace rdx;

bool BaseSystem::Init()
{
	//RX_PROFILE(GetName() + "Init");

	if (IsInitialized())
		return false;

	return m_IsInitialized = InitImpl();
}

bool BaseSystem::Terminate()
{
	//RX_PROFILE(GetName() + "Terminate");

	bool success = false;
	if (IsInitialized())
		success = TerminateImpl();

	m_IsInitialized = false;
	return success;
}

void BaseSystem::Update(float dt)
{
	RX_PROFILE(GetName() + "Update");

	UpdateImpl(dt);
}

void BaseSystem::Draw()
{
	RX_PROFILE(GetName() + "Draw");

	DrawImpl();
}

bool BaseUtil::Init()
{
	if (IsInitialized())
		return false;

	return m_IsInitialized = InitImpl();
}

bool BaseUtil::Terminate()
{
	bool success = false;
	if (IsInitialized())
		success = TerminateImpl();

	m_IsInitialized = false;
	return success;
}
