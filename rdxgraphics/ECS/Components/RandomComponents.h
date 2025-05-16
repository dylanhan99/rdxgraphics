#pragma once
#include "BaseComponent.h"

// Just a flag for editor control
class NoDelete : public BaseComponent
{
public:
	NoDelete() = default;
	~NoDelete() = default;
};