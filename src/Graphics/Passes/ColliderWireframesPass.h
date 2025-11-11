#pragma once
#include "Graphics/BasePass.h"

class ColliderWireframesPass : public BasePass
{
	_RX_DEF_RENDER_PASS(ColliderWireframesPass);
public:
	void DrawImpl() const override;

private:
};