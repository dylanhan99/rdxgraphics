#include "ServiceLayer.h"
#include "Window/GLFWWindow/GLFWWindow.h"

using namespace rdx;

bool ServiceLayer::Init()
{
	ServiceLayer* const sl = ServiceLayer::Get();
	sl->m_WindowService = new GLFWWindow{};
	sl->m_InputService = new Input{};

	bool success = true;
	success &= m_WindowService->Init();
	//success &= m_InputService->Init();

	return success;
}

bool ServiceLayer::Terminate()
{
	if (!m_ServiceLayer) 
		return false;

	bool success = true;
	//success &= m_InputService->Terminate();
	success &= m_WindowService->Terminate();

	// Deleting services
	delete m_ServiceLayer->m_WindowService;
	delete m_ServiceLayer->m_InputService;

	// Finally delete instance
	delete m_ServiceLayer;

	return success;
}

ServiceLayer* const ServiceLayer::Get()
{
	if (!m_ServiceLayer) m_ServiceLayer = new ServiceLayer{};
	return m_ServiceLayer;
}

BaseWindow* const ServiceLayer::WindowService()
{
	return m_ServiceLayer->m_WindowService;
}

Input* const ServiceLayer::InputService()
{
	return m_ServiceLayer->m_InputService;
}