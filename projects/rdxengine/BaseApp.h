#ifndef BASEAPP_H
#define BASEAPP_H
#include "RXAPI.h"
#include "BaseService.h"

namespace rdx
{
	class RX_API BaseApp : public BaseService
	{
	public:
		// Init
		// Terminate

		inline void FrameStart() { FrameStartImpl(); }
		inline void FrameEnd() { FrameEndImpl(); }

	private:
		virtual void FrameStartImpl() {};
		virtual void FrameEndImpl() {};
	};
}

#endif