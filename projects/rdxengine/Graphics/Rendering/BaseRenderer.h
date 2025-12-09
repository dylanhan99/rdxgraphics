#ifndef BASERENDERER_H
#define BASERENDERER_H
#include "RXAPI.h"
#include "BaseSystem.h"

namespace rdx
{
	struct CameraComponent;

	class RX_API BaseRenderer : public BaseSystem
	{
		RX_DECLARE_SYSTEM("Renderer", Renderer);
	public:
		virtual ~BaseRenderer() = default;

		void Draw();
		virtual void SetDepthTest(bool flag);

		CameraComponent* GetCurrentCamera() { return m_CurrentCamera; }

	private:
		virtual void DrawImpl() = 0;

	protected:
		CameraComponent* m_CurrentCamera{}; // This is assumed to be the game's camera fyi. Logic end can change cameras at will.
	};
}

#endif