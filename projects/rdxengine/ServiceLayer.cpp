#include "ServiceLayer.h"

using namespace rdx;

ServiceLayer::ServiceLayer(BaseWindow* pWindow, Input* pInput)
	: m_WindowService(pWindow), m_InputService(pInput)
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