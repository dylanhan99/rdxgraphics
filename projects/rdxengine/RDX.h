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

		inline bool IsInitialized() const { return m_IsInitialized; }
		void SetServiceLayer(ServiceLayer* sl);

	private:
		bool Init();
		bool Terminate();

	private:
		bool m_IsInitialized{ false }; // Change to some "Engine Status" enum in the future
	};
}

#endif