#include "ServiceLayer.h"

using namespace rdx;

RX_API ServiceLayer* ServiceLayer::s_ServiceLayer{};

ServiceLayer::ServiceLayer()
{}

void ServiceLayer::SetServiceLayer(ServiceLayer* pLayer)
{
	s_ServiceLayer = pLayer;
}

ServiceLayer* ServiceLayer::GetServiceLayer()
{
	return s_ServiceLayer;
}

BaseWindow* const ServiceLayer::WindowSystem()
{
	return static_cast<BaseWindow*>(s_ServiceLayer->m_Systems[static_cast<size_t>(SystemType::Window)]);
}

BaseRenderer* const ServiceLayer::RenderingSystem()
{
	return static_cast<BaseRenderer*>(s_ServiceLayer->m_Systems[static_cast<size_t>(SystemType::Renderer)]);
}

Input* const ServiceLayer::InputService()
{
	return static_cast<Input*>(s_ServiceLayer->m_Utils[static_cast<size_t>(UtilType::Input)]);
}

BaseLogger* const ServiceLayer::LoggingService()
{
	return static_cast<BaseLogger*>(s_ServiceLayer->m_Utils[static_cast<size_t>(UtilType::Logger)]);
}

InstantEventBus* const ServiceLayer::InstantEventService()
{
	return static_cast<InstantEventBus*>(s_ServiceLayer->m_Utils[static_cast<size_t>(UtilType::InstantEvent)]);
}

BaseEntityComponentWorld* const ServiceLayer::EntityComponentService()
{
	return static_cast<BaseEntityComponentWorld*>(s_ServiceLayer->m_Utils[static_cast<size_t>(UtilType::ECS)]);
}

PerformanceProfiler* const ServiceLayer::PerformanceProfilingService()
{
	return static_cast<PerformanceProfiler*>(s_ServiceLayer->m_Utils[static_cast<size_t>(UtilType::PerformanceProfiler)]);
}

BaseApp* const ServiceLayer::ApplicationService()
{
	return s_ServiceLayer->m_App;
}

std::array<BaseSystem*, static_cast<size_t>(SystemType::MAX)>& ServiceLayer::GetSystems()
{
	return s_ServiceLayer->m_Systems;
}

std::array<BaseUtil*, static_cast<size_t>(UtilType::MAX)>& ServiceLayer::GetUtils()
{
	return s_ServiceLayer->m_Utils;
}

BaseApp* ServiceLayer::GetApp()
{
	return s_ServiceLayer->m_App;
}