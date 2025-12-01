#ifndef BASERENDERER_H
#define BASERENDERER_H
#include "RXAPI.h"
#include "BaseSystem.h"
#include "ECS/Components/CameraComponent.h"

namespace rdx
{
	class RX_API BaseRenderer : public BaseSystem
	{
		RX_DECLARE_SYSTEM("Renderer", Renderer);
	public:
		virtual ~BaseRenderer() = default;

		void Draw();
		virtual void SetDepthTest(bool flag);

	private:
		virtual void DrawImpl() = 0;

	protected:
		CameraComponent m_EditorCamera{};
	};
}

#endif