#include "ServiceLayer.h"

using namespace rdx;

void ServiceLayer::Init()
{
	ServiceLayer* const sl = ServiceLayer::Get();
	sl->m_InputService = new Input{};
}

void ServiceLayer::Terminate()
{
	if (!m_ServiceLayer) 
		return;

	// Deleting services
	delete m_ServiceLayer->m_InputService;

	// Finally delete instance
	delete m_ServiceLayer;
}

ServiceLayer* const ServiceLayer::Get()
{
	if (!m_ServiceLayer) m_ServiceLayer = new ServiceLayer{};
	return m_ServiceLayer;
}

Input* const ServiceLayer::InputService()
{
	return m_ServiceLayer->m_InputService;
}