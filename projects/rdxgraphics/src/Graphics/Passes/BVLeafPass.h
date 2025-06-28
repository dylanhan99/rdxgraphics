#pragma once
#include "Graphics/BasePass.h"

class BVLeafPass : public BasePass
{
	_RX_DEF_RENDER_PASS(BVLeafPass);
public:
	void DrawImpl() const override;

private:
};