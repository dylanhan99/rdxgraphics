#ifndef CRAPPYRENDERER_H
#define CRAPPYRENDERER_H
#include "Graphics/BaseRenderer.h"

namespace rdx
{
	class CrappyRenderer : public BaseRenderer
	{
	public:
		bool InitImpl() override;
		bool TerminateImpl() override;

	private:
		void DrawImpl() override;

	private:
		void SetupGlewDefaults();
		void SetupDefaultAssets();
	};
}

#endif