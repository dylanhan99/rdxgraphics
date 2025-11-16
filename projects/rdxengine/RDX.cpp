#include "RDX.h"
#include "ServiceLayer.h"

using namespace rdx;

RDX::RDX()
{
	ServiceLayer::Get();
}

RDX::~RDX()
{

}

int RDX::Run()
{
	int exitCode = EXIT_SUCCESS;

	if (ServiceLayer::Get()->Init())
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

	if (!ServiceLayer::Get()->Terminate())
	{
		std::cerr << "Failed to terminate something. Exiting." << std::endl;
		exitCode = EXIT_FAILURE;
	}

	return exitCode;
}