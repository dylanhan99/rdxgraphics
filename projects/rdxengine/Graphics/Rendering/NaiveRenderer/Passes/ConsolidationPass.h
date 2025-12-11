#ifndef CONSOLIDATIONPASS_H
#define CONSOLIDATIONPASS_H
#include "../NaiveBasePass.h"

namespace rdx
{
	class ConsolidationPass : public NaiveBasePass
	{
	public:
		ConsolidationPass(CameraComponent* camera = nullptr) : NaiveBasePass(camera) {}

	private:
		bool InitImpl() override;
		bool TerminateImpl() override;
		void DrawImpl() override;

	private:
		NaiveRenderer::Shader m_Shader{};
		NaiveRenderer::Mesh m_ScreenMesh{};
	};
}

#endif