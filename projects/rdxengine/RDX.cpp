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
				ServiceLayer::WindowSystem()->SetShouldClose();
			});

		static uint64_t frame = 0;
		auto lastTime = std::chrono::system_clock::now();
		float dt{};
		constexpr int targetFrames = 30;
		constexpr float targetDt = 1.f / targetFrames;
		int currentFPS{};
		while (!ServiceLayer::WindowSystem()->IsWindowShouldClose())
		{
			++frame;
			{
				auto now = std::chrono::system_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - lastTime);
				lastTime = now;
				dt = elapsed.count() * 0.001f; // Convert ms to seconds
			}
			{ // FPS value displayer. Resets every 1 second
				static float dtAccum = 0.f;
				static int numFrames = 0;

				dtAccum += dt;
				++numFrames;

				if (dtAccum >= 1000.f)
				{
					currentFPS = dtAccum / numFrames;
					dtAccum = 0.f;
					numFrames = 0;
					std::cout << currentFPS << "\n";
				}
			}

			RX_PROFILE_FRAME(frame);
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

#if 0
			{
				static float accum{};
				accum += dt;
				std::cout << accum << "\n";
			}
#endif
			
			{
				// Becareful of a pure sleep_until approach. The OS may sleep for +-[2,5]ms, which is inconsequential at lower fps
				// But at higher target fps, the 2ms really adds up, maybe a 244fps may only perform at 150+fps
				// Not really important right now, but something to thinka bout next time.
				using namespace std::chrono;
				auto end = lastTime + round<milliseconds>(duration<float>(targetDt)); // The "correct" next frame time
				std::this_thread::sleep_until(end);
			}
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