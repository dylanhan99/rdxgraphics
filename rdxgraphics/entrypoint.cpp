#include <pch.h>
#include "RDX.h"

int main()
{
	try {
		RDX::Run();
	} catch (std::exception const& e) {
		std::cerr << e.what() << std::endl;
	} catch (...) {
		std::cerr << "Unknown Exception" << std::endl;
	}

	return EXIT_SUCCESS;
}
