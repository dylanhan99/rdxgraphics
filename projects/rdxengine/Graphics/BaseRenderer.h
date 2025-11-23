#ifndef BASERENDERER_H
#define BASERENDERER_H
#include "BaseService.h"

namespace rdx
{
	class BaseRenderer : public BaseService
	{
	public:
		void Draw() { DrawImpl(); }

	private:
		virtual void DrawImpl() = 0;
	};
}

#endif