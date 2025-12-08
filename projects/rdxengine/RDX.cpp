#include "RDX.h"
#include "ServiceLayer.h"
#include "Window/GLFWWindow.h"
#include "Logging/AsyncLogger.h"
#include "ECS/Worlds/EnttWorld.h"
#include "ECS/Entity.h"

//#include "Graphics/Rendering/CrappyRenderer/CrappyRenderer.h"
#include "Graphics/Rendering/NaiveRenderer/NaiveRenderer.h"

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
				ServiceLayer::WindowSystem()->SetShouldClose();
			});

		while (!ServiceLayer::WindowSystem()->IsWindowShouldClose())
		{
			ServiceLayer::FrameRateControllerService()->FrameStart();
			const float dt = ServiceLayer::FrameRateControllerService()->GetDT();

			RX_PROFILE_FRAME(ServiceLayer::FrameRateControllerService()->GetFrame());
			RX_PROFILE("Main Loop");
			ServiceLayer::WindowSystem()->PollEvents();

			ServiceLayer::ApplicationService()->FrameStart();

			{
				if (ServiceLayer::InputService()->IsKeyTriggered(KeyCode::Escape))
				{
					ServiceLayer::InstantEventService()->Publish(ShutdownEngineEvent{ "ESCAPE!" });
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
			}

			for (BaseSystem* p : ServiceLayer::GetSystems())
				p->Update(dt);

			for (BaseSystem* p : ServiceLayer::GetSystems())
				p->Draw();

			ServiceLayer::PerformanceProfilingService()->Update(dt);
			ServiceLayer::ApplicationService()->FrameEnd();
			ServiceLayer::WindowSystem()->SwapBuffers();

			ServiceLayer::FrameRateControllerService()->FrameEnd();
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
	for (BaseUtil* p : ServiceLayer::GetUtils())
		success &= p->Init();
	for (BaseSystem* p : ServiceLayer::GetSystems())
		success &= p->Init();
	success &= ServiceLayer::GetApp()->Init();

	return success;
}

bool RDX::Terminate()
{
	RX_ASSERT(ServiceLayer::GetServiceLayer());

	bool success = true;
	success &= ServiceLayer::GetApp()->Terminate();
	for (auto it = ServiceLayer::GetSystems().rbegin(); it != ServiceLayer::GetSystems().rend(); ++it)
		success &= (*it)->Terminate();
	for (auto it = ServiceLayer::GetUtils().rbegin(); it != ServiceLayer::GetUtils().rend(); ++it)
		success &= (*it)->Terminate();

	return success;
}