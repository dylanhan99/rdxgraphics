#include "RDX.h"
#include "ServiceLayer.h"
#include "Window/GLFWWindow.h"
#include "Logging/AsyncLogger.h"

using namespace rdx;

RDX::RDX()
{
	m_Window = std::make_unique<GLFWWindow>();
	m_Input = std::make_unique<Input>();
	m_Logging = std::make_unique<AsyncLogger>();

	m_ServiceLayer = std::make_unique<ServiceLayer>(m_Window.get(), m_Input.get(), m_Logging.get());
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
		while (!ServiceLayer::WindowService()->IsWindowShouldClose())
		{
			ServiceLayer::WindowService()->PollEvents();

			if (ServiceLayer::InputService()->IsKeyTriggered(KeyCode::Escape))
				ServiceLayer::WindowService()->SetShouldClose();

			if (ServiceLayer::InputService()->IsKeyTriggered(KeyCode::A))
			{
				for (int i = 0; i < 1000; ++i)
				{
					RX_TRACE("Test {}", i);
				}
			}
		}
	}
	else
	{
		std::cerr << "Failed to initialize something. Exiting." << std::endl;
		exitCode = EXIT_FAILURE;
	}

	if (!Terminate())
	{
		std::cerr << "Failed to terminate something. Exiting." << std::endl;
		exitCode = EXIT_FAILURE;
	}

	return exitCode;
}

bool RDX::Init()
{
	bool success = true;
	success &= m_Window->Init();
	//success &= m_Input->Init();
	success &= m_Logging->Init();

	return success;
}

bool RDX::Terminate()
{
	bool success = true;
	success &= m_Logging->Terminate();
	//success &= m_Input->Terminate();
	success &= m_Window->Terminate();

	return success;
}