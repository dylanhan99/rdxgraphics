#ifndef BASERENDERER_H
#define BASERENDERER_H
#include "RXAPI.h"
#include "BaseService.h"
#include "ECS/Components/CameraComponent.h"

namespace rdx
{
	class RX_API BaseRenderer : public BaseService
	{
	public:
		inline const char* GetName() const override { return "Renderer"; }

	public:
		void Draw();
		virtual void SetDepthTest(bool flag);

	private:
		virtual void DrawImpl() = 0;

	protected:
		CameraComponent m_EditorCamera{};
	};
}

#endif