#pragma once
#include "Graphics/BasePass.h"

class ScreenPass : public BasePass
{
	_RX_DEF_RENDER_PASS(ScreenPass);
public:
	void DrawImpl() const override;

private:
};