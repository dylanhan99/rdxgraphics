#pragma once
#include "Graphics/BasePass.h"

class BVWireframesPass : public BasePass
{
	_RX_DEF_RENDER_PASS(BVWireframesPass);
public:
	void DrawImpl() const override;

private:
};