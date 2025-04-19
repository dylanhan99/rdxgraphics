#include <pch.h>
#include "FramerateController.h"

RX_SINGLETON_EXPLICIT(FramerateController);

void FramerateController::Init(uint32_t targetFPS)
{
	SetTargetFPS(targetFPS);
}

void FramerateController::Update(std::function<void(double)> fnUpdate)
{
	if (g.m_AccumulatedDT <= g.m_TargetDT)
		return;

	StartFrame();
	if (fnUpdate) fnUpdate(GetDT());
	g.m_AccumulatedDT = 0.0;
}

void FramerateController::StartGameLoop()
{
	g.m_PrevTime = g.m_CurrTime;
	g.m_CurrTime = glfwGetTime();
	double dt = g.m_CurrTime - g.m_PrevTime;

	g.m_AccumulatedDT += dt;
}

void FramerateController::StartFrame()
{
	g.m_FPS = static_cast<uint32_t>(1.0 / g.m_AccumulatedDT);
	g.m_IntervalTimer += GetDT();
	if (g.m_IntervalTimer >= 1.0)
	{
		g.m_IntervalTimer = 0.0;
		g.m_IntervalFPS = g.m_FPS;
	}
}

void FramerateController::SetTargetFPS(uint32_t target)
{
	g.m_TargetFPS = target;
	g.m_TargetDT = 1.0 / g.m_TargetFPS;
}
