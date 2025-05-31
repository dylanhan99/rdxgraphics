#pragma once
#include "Graphics/BasePass.h"

class ScreenPass : public BasePass
{
public:
	inline ScreenPass(std::string const& name) : BasePass(name) {}
	void DrawImpl() const override;

private:
};