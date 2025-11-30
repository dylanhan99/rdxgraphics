#include "BaseService.h"
#include "Profiling/PerformanceProfiler.h"

using namespace rdx;

bool BaseService::Init()
{
	RX_PROFILE_ENTER(GetName());
	RX_PROFILE("Initialization");

	if (IsInitialized())
		return false;

	return m_IsInitialized = InitImpl();
}

bool BaseService::Terminate()
{
	RX_PROFILE_ENTER(GetName());
	RX_PROFILE("Terminate");

	bool success = false;
	if (IsInitialized())
		success = TerminateImpl();

	m_IsInitialized = false;
	return success;
}

void BaseService::Update(float dt)
{
	RX_PROFILE_ENTER(GetName());
	RX_PROFILE("Update");

	UpdateImpl(dt);
}

void BaseService::Draw()
{
	RX_PROFILE_ENTER(GetName());
	RX_PROFILE("Draw");

	DrawImpl();
}