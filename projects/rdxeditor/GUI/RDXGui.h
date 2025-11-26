#ifndef RDXGUI_H
#define RDXGUI_H
#include "rdxengine/BaseApp.h"

namespace rdxgui
{
	class RDXGui : public rdx::BaseApp
	{
	private:
		bool InitImpl() override;
		bool TerminateImpl() override;

		void FrameStartImpl() override;
		void FrameEndImpl() override;
	};
}

#endif