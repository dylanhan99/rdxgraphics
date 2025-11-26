#include "ServiceLayer.h"

using namespace rdx;

RX_API ServiceLayer* ServiceLayer::s_ServiceLayer{};

ServiceLayer::ServiceLayer(BaseWindow* pWindow, Input* pInput, BaseLogger* pLogger, InstantEventBus* pInstantEventBus, BaseEntityComponentWorld* pEntityComponentWorld, BaseRenderer* pRenderer, BaseApp* pApp)
	: m_WindowService(pWindow), m_InputService(pInput), m_LoggingService(pLogger), m_InstantEventService(pInstantEventBus), m_EntityComponentService(pEntityComponentWorld), m_RenderingService(pRenderer), m_ApplicationService(pApp)
{}

void ServiceLayer::SetServiceLayer(ServiceLayer* pLayer)
{
	s_ServiceLayer = pLayer;
}

ServiceLayer* ServiceLayer::GetServiceLayer()
{
	return s_ServiceLayer;
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

BaseApp* const ServiceLayer::ApplicationService()
{
	return s_ServiceLayer->m_ApplicationService;
}