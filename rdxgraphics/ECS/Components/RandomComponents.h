#pragma once
#include "BaseComponent.h"

class Metadata : public BaseComponent
{
public:
	inline std::string& GetName() { return m_Name; }

private:
	std::string m_Name{};
};

// Just a flag for editor control
class NoDelete : public BaseComponent
{
public:
	NoDelete() = default;
	~NoDelete() = default;
};