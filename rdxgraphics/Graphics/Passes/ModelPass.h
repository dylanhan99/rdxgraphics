#pragma once
#include "Graphics/BasePass.h"

class ModelPass : public BasePass
{
public:
	inline ModelPass(std::string const& name) : BasePass(name) {}
	void DrawImpl() const override;

private:
};