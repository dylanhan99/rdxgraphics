#pragma once
#include "BaseComponent.h"

class Metadata : public BaseComponent
{
	RX_COMPONENT_HAS_HANDLE(Metadata);
public:
	Metadata() = default;
	inline Metadata(std::string const& name)
		: m_Name(name) {}

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