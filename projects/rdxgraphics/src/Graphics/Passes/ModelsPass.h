#pragma once
#include "Graphics/BasePass.h"

class ModelsPass : public BasePass
{
	_RX_DEF_RENDER_PASS(ModelsPass);
public:
	void DrawImpl() const override;

private:
};