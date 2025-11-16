#include "ServiceLayer.h"

using namespace rdx;

ServiceLayer::ServiceLayer(BaseWindow* pWindow, Input* pInput, BaseLogger* pLogger)
	: m_WindowService(pWindow), m_InputService(pInput), m_LoggingService(pLogger)
{}

void ServiceLayer::RegisterServiceLayer(ServiceLayer* pLayer)
{
	s_ServiceLayer = pLayer;
}

BaseWindow* const ServiceLayer::WindowService()
{
	return s_ServiceLayer->m_WindowService;
}

Input* const ServiceLayer::InputService()
{
	return s_ServiceLayer->m_InputService;
}

BaseLogger* const ServiceLayer::LoggingService()
{
	return s_ServiceLayer->m_LoggingService;
}
