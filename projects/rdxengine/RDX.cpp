#include "RDX.h"
#include "Window/GLFWWindow/GLFWWindow.h"

using namespace rdx;

RDX::RDX()
{
	m_window = new GLFWWindow{};
}

RDX::~RDX()
{
	delete m_window;
}

int RDX::Run()
{
	int exitCode = EXIT_SUCCESS;

	if (m_window->Init())
	{
		while (!m_window->IsWindowShouldClose())
		{
			m_window->PollEvents();
		}
	}
	else
	{
		std::cerr << "Failed to initialize something. Exiting." << std::endl;
		exitCode = EXIT_FAILURE;
	}

	if (!m_window->Terminate())
	{
		std::cerr << "Failed to terminate something. Exiting." << std::endl;
		exitCode = EXIT_FAILURE;
	}

	return exitCode;
}