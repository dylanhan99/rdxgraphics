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
	m_Window = std::make_unique<GLFWWindow>();
	m_Input = std::make_unique<Input>();
	m_Logging = std::make_unique<AsyncLogger>();
	m_InstantEventBus = std::make_unique<InstantEventBus>();
	m_EntityComponentWorld = std::make_unique<EnttWorld>();
	m_Renderer = std::make_unique<NaiveRenderer>();

	m_ServiceLayer = std::make_unique<ServiceLayer>(m_Window.get(), m_Input.get(), m_Logging.get(), m_InstantEventBus.get(), m_EntityComponentWorld.get(), m_Renderer.get());
	m_ServiceLayer->RegisterServiceLayer(m_ServiceLayer.get());
}

RDX::~RDX()
{
}

int RDX::Run()
{
	int exitCode = EXIT_SUCCESS;

	if (Init())
	{
		ServiceLayer::InstantEventService()->Subscribe<ShutdownEngineEvent>(
			[](ShutdownEngineEvent const& e)
			{
				RX_INFO("Shutting down yo mama because: {}", e.Reason);
				ServiceLayer::WindowService()->SetShouldClose();
			});

		ServiceLayer::RenderingService()->Init();

		while (!ServiceLayer::WindowService()->IsWindowShouldClose())
		{
			ServiceLayer::WindowService()->PollEvents();

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
				constexpr int sz = 100;
				for (int i = -sz/2; i < sz/2; ++i)
				{
					Entity ent = ServiceLayer::EntityComponentService()->CreateEntity();
					auto pXform = ent.AddComponent<TransformComponent>();
					if (pXform)
					{
						auto& xform = *pXform;
						xform.Position.x = i;
						xform.Position.y = i;
						xform.Position.z = 0.f;
						//RX_DEBUG("{}, {}, {}", xform.Position.x, xform.Position.y, xform.Position.z);
					}
				}
			}

			ServiceLayer::RenderingService()->Draw();
			ServiceLayer::WindowService()->SwapBuffers();
		}

		ServiceLayer::RenderingService()->Terminate();
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

bool RDX::Init()
{
	bool success = true;
	success &= m_Window->Init();
	success &= m_Input->Init();
	success &= m_Logging->Init();
	success &= m_InstantEventBus->Init();
	success &= m_EntityComponentWorld->Init();

	return success;
}

bool RDX::Terminate()
{
	bool success = true;
	success &= m_EntityComponentWorld->Terminate();
	success &= m_InstantEventBus->Terminate();
	success &= m_Logging->Terminate();
	success &= m_Input->Terminate();
	success &= m_Window->Terminate();

	return success;
}