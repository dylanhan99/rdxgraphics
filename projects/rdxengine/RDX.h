#ifndef RDX_H
#define RDX_H
#include "RXAPI.h"
#include "ServiceLayer.h"

namespace rdx
{
	class RX_API RDX
	{
	public:
		RDX();
		~RDX();
		int Run();

	private:
		bool Init();
		bool Terminate();

	private:
		std::unique_ptr<ServiceLayer> m_ServiceLayer{};

		std::unique_ptr<BaseWindow> m_Window{};
		std::unique_ptr<Input> m_Input{};
		std::unique_ptr<BaseLogger> m_Logging{};
		std::unique_ptr<InstantEventBus> m_InstantEventBus{};
	};
}

#endif