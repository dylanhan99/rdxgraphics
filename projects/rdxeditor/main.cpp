#include "rdxengine/RDX.h"
#include "GUI/RDXGui.h"

#include "rdxengine/Window/GLFWWindow.h"
#include "rdxengine/Input/Input.h"
#include "rdxengine/Logging/AsyncLogger.h"
#include "rdxengine/Event/Busses/InstantEventBus.h"
#include "rdxengine/ECS/Worlds/EnttWorld.h"
#include "rdxengine/Graphics/Rendering/NaiveRenderer/NaiveRenderer.h"
#include "rdxengine/Utils/FrameRateController.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include "rdxengine/Graphics/MultiPassing/Passes/TestPass.h"

extern std::shared_ptr<rdx::TestPass> editorPass;
extern std::shared_ptr<rdx::TestPass> gamePass;

int main()
{
	using namespace rdx;
	using namespace rdxgui;

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	int exitCode = EXIT_SUCCESS;
	try {
		//
		std::unique_ptr<GLFWWindow>		 m_Window = std::make_unique<GLFWWindow>();
		std::unique_ptr<NaiveRenderer>	 m_Renderer = std::make_unique<NaiveRenderer>();

		//
		std::unique_ptr<Input>					m_Input = std::make_unique<Input>();
		std::unique_ptr<AsyncLogger>			m_Logging = std::make_unique<AsyncLogger>();
		std::unique_ptr<InstantEventBus>		m_InstantEventBus = std::make_unique<InstantEventBus>();
		std::unique_ptr<EnttWorld>				m_EntityComponentWorld = std::make_unique<EnttWorld>();
		std::unique_ptr<PerformanceProfiler>	m_Profiler = std::make_unique<PerformanceProfiler>();
		std::unique_ptr<FrameRateController>	m_FRC = std::make_unique<FrameRateController>();

		//
		std::unique_ptr<RDXGui>	m_App = std::make_unique<RDXGui>();

		// Passes
		std::shared_ptr<TestPass> m_EditorTestPass = std::make_shared<TestPass>(m_App->GetCamera());
		std::shared_ptr<TestPass> m_GameTestPass = std::make_shared<TestPass>();
		{
			{
				rdx::RenderPipeline editorPipe{};
				editorPipe.RegisterPass(m_EditorTestPass);

				m_Renderer->RegisterPipeline(editorPipe);
				editorPass = m_EditorTestPass; // This is the pass you wanna see in the viewport
			}
			{
				rdx::RenderPipeline gamePipe{};
				gamePipe.RegisterPass(m_GameTestPass);

				m_Renderer->RegisterPipeline(gamePipe);
				gamePass = m_GameTestPass; // This is the pass you wanna see in the viewport
			}
		}

		std::shared_ptr<ServiceLayer> serviceLayer = std::make_shared<ServiceLayer>();
		serviceLayer->RegisterSystem(m_Window.get());
		serviceLayer->RegisterSystem(m_Renderer.get());

		serviceLayer->RegisterUtil(m_Input.get());
		serviceLayer->RegisterUtil(m_Logging.get());
		serviceLayer->RegisterUtil(m_InstantEventBus.get());
		serviceLayer->RegisterUtil(m_EntityComponentWorld.get());
		serviceLayer->RegisterUtil(m_Profiler.get());
		serviceLayer->RegisterUtil(m_FRC.get());

		serviceLayer->RegisterApp(m_App.get());

		//std::shared_ptr<ServiceLayer> serviceLayer = std::make_unique<ServiceLayer>(
		//	m_Window.get(),
		//	m_Input.get(),
		//	m_Logging.get(),
		//	m_InstantEventBus.get(),
		//	m_EntityComponentWorld.get(),
		//	m_Renderer.get(),
		//	m_App.get(),
		//	m_Profiler.get()
		//);

		rdx::RDX engine{};
		engine.SetServiceLayer(serviceLayer.get());
		exitCode = engine.Run();
	} catch (std::exception const& e) {
		std::cerr << e.what() << std::endl;
	} catch (...) {
		std::cerr << "Unknown Exception" << std::endl;
	}
	return exitCode;
}
