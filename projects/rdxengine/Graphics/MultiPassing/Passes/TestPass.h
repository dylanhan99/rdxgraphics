#ifndef TESTPASS_H
#define TESTPASS_H
#include "Graphics/MultiPassing/BasePass.h"

namespace rdx
{
	class TestPass : public BasePass
	{
	public:
		TestPass(CameraComponent* camera = nullptr) : BasePass(camera) {}
		
	private:
		void DrawImpl() override;
	};
}

#endif