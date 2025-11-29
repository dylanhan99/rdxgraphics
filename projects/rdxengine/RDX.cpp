#include "RDX.h"
#include "ServiceLayer.h"
#include "Window/GLFWWindow.h"
#include "Logging/AsyncLogger.h"
#include "ECS/Worlds/EnttWorld.h"
#include "ECS/Entity.h"

//#include "Graphics/CrappyRenderer/CrappyRenderer.h"
#include "Graphics/NaiveRenderer/NaiveRenderer.h"

#include "Event/Events/Events.h"

using namespace rdx;

RDX::RDX()
{
}

RDX::~RDX()
{
}

int RDX::Run()
{
	int exitCode = EXIT_SUCCESS;

	if (IsInitialized())
	{
		ServiceLayer::InstantEventService()->Subscribe<ShutdownEngineEvent>(
			[](ShutdownEngineEvent const& e)
			{
				RX_INFO("Shutting down yo mama because: {}", e.Reason);
				ServiceLayer::WindowService()->SetShouldClose();
			});

		while (!ServiceLayer::WindowService()->IsWindowShouldClose())
		{
			RX_PROFILE("Main Loop");
			ServiceLayer::WindowService()->PollEvents();

			ServiceLayer::ApplicationService()->FrameStart();

			if (ServiceLayer::InputService()->IsKeyTriggered(KeyCode::Escape))
			{
				ServiceLayer::InstantEventService()->Publish(ShutdownEngineEvent{"ESCAPE!"});
			}

			if (ServiceLayer::InputService()->IsKeyTriggered(KeyCode::A))
			{
				for (int i = 0; i < 1000; ++i)
				{
					RX_TRACE("Test {}", i);
				}
			}

			if (ServiceLayer::InputService()->IsKeyTriggered(KeyCode::S))
			{
				ServiceLayer::EntityComponentService()->Clear();
			}

			if (ServiceLayer::InputService()->IsKeyTriggered(KeyCode::D))
			{
				//constexpr int sz = 3;
				//for (int i = -sz/2; i < sz/2; ++i)
				//{
				//	Entity ent = ServiceLayer::EntityComponentService()->CreateEntity();
				//	auto pXform = ent.AddComponent<TransformComponent>();
				//	if (pXform)
				//	{
				//		auto& xform = *pXform;
				//		xform.Position.x = i;
				//		xform.Position.y = i;
				//		xform.Position.z = 0.f;
				//		//RX_DEBUG("{}, {}, {}", xform.Position.x, xform.Position.y, xform.Position.z);
				//	}
				//}
				{
					Entity ent = ServiceLayer::EntityComponentService()->CreateEntity();
					auto pXform = ent.AddComponent<TransformComponent>();
					if (pXform)
					{
						auto& xform = *pXform;
						xform.Position.x = -1.f;
						xform.Position.y = -1.f;
						xform.Position.z = -1.f;
					}
				}
				{
					Entity ent = ServiceLayer::EntityComponentService()->CreateEntity();
					auto pXform = ent.AddComponent<TransformComponent>();
					if (pXform)
					{
						auto& xform = *pXform;
						xform.Position.x = 0.f;
						xform.Position.y = 0.f;
						xform.Position.z = 0.f;
					}
				}
				{
					Entity ent = ServiceLayer::EntityComponentService()->CreateEntity();
					auto pXform = ent.AddComponent<TransformComponent>();
					if (pXform)
					{
						auto& xform = *pXform;
						xform.Position.x = 1.f;
						xform.Position.y = 1.f;
						xform.Position.z = 1.f;
					}
				}
			}

			ServiceLayer::RenderingService()->Draw();
			ServiceLayer::ApplicationService()->FrameEnd();
			ServiceLayer::WindowService()->SwapBuffers();
		}
	}
	else
	{
		RX_CRITICAL("Failed to INITIALIZE something. Exiting.");
		exitCode = EXIT_FAILURE;
	}

	if (!Terminate())
	{
		RX_CRITICAL("Failed to TERMINATE something. Exiting.");
		exitCode = EXIT_FAILURE;
	}


	return exitCode;
}

void RDX::SetServiceLayer(ServiceLayer* sl)
{
	RX_ASSERT(sl);

	bool success{};
	if (ServiceLayer::GetServiceLayer())
	{
		success = Terminate();
		RX_ASSERT(success);
	}

	ServiceLayer::SetServiceLayer(sl);

	success = Init();
	m_IsInitialized = success;
}

bool RDX::Init()
{
	RX_ASSERT(ServiceLayer::GetServiceLayer());

	bool success = true;
	success &= ServiceLayer::WindowService()->Init();
	//if (ServiceLayer::WindowService()->IsInitialized())
	//	ServiceLayer::WindowService()->PollEvents();

	success &= ServiceLayer::InputService()->Init();
	success &= ServiceLayer::LoggingService()->Init();
	success &= ServiceLayer::InstantEventService()->Init();
	success &= ServiceLayer::EntityComponentService()->Init();
	success &= ServiceLayer::RenderingService()->Init();
	success &= ServiceLayer::ApplicationService()->Init();

	return success;
}

bool RDX::Terminate()
{
	RX_ASSERT(ServiceLayer::GetServiceLayer());

	bool success = true;
	success &= ServiceLayer::ApplicationService()->Terminate();
	success &= ServiceLayer::RenderingService()->Terminate();
	success &= ServiceLayer::EntityComponentService()->Terminate();
	success &= ServiceLayer::InstantEventService()->Terminate();
	success &= ServiceLayer::LoggingService()->Terminate();
	success &= ServiceLayer::InputService()->Terminate();
	success &= ServiceLayer::WindowService()->Terminate();

	return success;
}