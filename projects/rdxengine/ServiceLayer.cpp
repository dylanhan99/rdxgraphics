#include "ServiceLayer.h"

using namespace rdx;

ServiceLayer::ServiceLayer(BaseWindow* pWindow, Input* pInput, BaseLogger* pLogger, InstantEventBus* pInstantEventBus, BaseEntityComponentWorld* pEntityComponentWorld, BaseRenderer* pRenderer)
	: m_WindowService(pWindow), m_InputService(pInput), m_LoggingService(pLogger), m_InstantEventService(pInstantEventBus), m_EntityComponentService(pEntityComponentWorld), m_RenderingService(pRenderer)
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

InstantEventBus* const ServiceLayer::InstantEventService()
{
	return s_ServiceLayer->m_InstantEventService;
}

BaseEntityComponentWorld* const ServiceLayer::EntityComponentService()
{
	return s_ServiceLayer->m_EntityComponentService;
}

BaseRenderer* const ServiceLayer::RenderingService()
{
	return s_ServiceLayer->m_RenderingService;
}