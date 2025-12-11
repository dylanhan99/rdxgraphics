#ifndef LIGHTINGPASS_H
#define LIGHTINGPASS_H
#include "../NaiveBasePass.h"

namespace rdx
{
	class LightingPass : public NaiveBasePass
	{
	public:
		LightingPass(CameraComponent* camera = nullptr) : NaiveBasePass(camera) {}

	private:
		bool InitImpl() override;
		bool TerminateImpl() override;
		void DrawImpl() override;

	private:
		NaiveRenderer::Shader m_Shader{};
	};
}

#endif