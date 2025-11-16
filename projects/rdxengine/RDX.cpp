#include "RDX.h"
#include "ServiceLayer.h"
#include "Window/GLFWWindow/GLFWWindow.h"

using namespace rdx;

RDX::RDX()
{
	m_Window = std::make_unique<GLFWWindow>();
	m_Input = std::make_unique<Input>();

	m_ServiceLayer = std::make_unique<ServiceLayer>(m_Window.get(), m_Input.get());
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

	return success;
}

bool RDX::Terminate()
{
	bool success = true;
	//success &= m_Input->Terminate();
	success &= m_Window->Terminate();

	return success;
}