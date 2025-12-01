#ifndef BASEAPP_H
#define BASEAPP_H
#include "RXAPI.h"
#include "BaseService.h"

namespace rdx
{
	class RX_API BaseApp : public BaseService
	{
	public:
		virtual ~BaseApp() = default;

		virtual bool Init() { return true; }
		virtual bool Terminate() { return true; }

		inline void FrameStart() { FrameStartImpl(); }
		inline void FrameEnd() { FrameEndImpl(); }

	private:
		virtual void FrameStartImpl() {};
		virtual void FrameEndImpl() {};
	};
}

#endif