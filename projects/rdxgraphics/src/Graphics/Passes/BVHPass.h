#pragma once
#include "Graphics/BasePass.h"

class BVHPass : public BasePass
{
	_RX_DEF_RENDER_PASS(BVHPass);
public:
	void DrawImpl() const override;

private:
};