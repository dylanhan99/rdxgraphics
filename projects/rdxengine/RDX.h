#ifndef RDX_H
#define RDX_H
#include "RXAPI.h"

namespace rdx
{
	class BaseWindow;

	class RX_API RDX
	{
	public:
		RDX();
		~RDX();
		int Run();

	private:
		BaseWindow* m_window{};
	};
}

#endif