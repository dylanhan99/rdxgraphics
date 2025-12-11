#ifndef TESTPASS_H
#define TESTPASS_H
#include "../NaiveBasePass.h"

namespace rdx
{
	class TestPass : public NaiveBasePass
	{
	public:
		TestPass(CameraComponent* camera = nullptr) : NaiveBasePass(camera) {}
		
	private:
		void DrawImpl() override;
	};
}

#endif