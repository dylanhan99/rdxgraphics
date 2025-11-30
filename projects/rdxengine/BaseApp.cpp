#include "BaseSystem.h"
#include "BaseUtil.h"
#include "Profiling/PerformanceProfiler.h"

using namespace rdx;

bool BaseSystem::Init()
{
	RX_PROFILE_ENTER(GetName());
	RX_PROFILE("Initialization");

	if (IsInitialized())
		return false;

	return m_IsInitialized = InitImpl();
}

bool BaseSystem::Terminate()
{
	RX_PROFILE_ENTER(GetName());
	RX_PROFILE("Terminate");

	bool success = false;
	if (IsInitialized())
		success = TerminateImpl();

	m_IsInitialized = false;
	return success;
}

void BaseSystem::Update(float dt)
{
	RX_PROFILE_ENTER(GetName());
	RX_PROFILE("Update");

	UpdateImpl(dt);
}

void BaseSystem::Draw()
{
	RX_PROFILE_ENTER(GetName());
	RX_PROFILE("Draw");

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
