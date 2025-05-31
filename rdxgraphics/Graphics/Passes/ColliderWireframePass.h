#pragma once
#include "Graphics/BasePass.h"

class ColliderWireframePass : public BasePass
{
public:
	inline ColliderWireframePass(std::string const& name) : BasePass(name) {}
	void DrawImpl() const override;

private:
};