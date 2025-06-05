#include <pch.h>
#include "Rxuid.h"

template<>
struct std::hash<std::chrono::system_clock::time_point>
{
	std::size_t operator()(std::chrono::system_clock::time_point const& d) const noexcept
	{
		return d.time_since_epoch().count();
	}
};
static std::hash<std::string> s_StrHash{};
static std::hash<std::chrono::time_point<std::chrono::system_clock>> s_TpHash{};

Rxuid::Rxuid()
	: m_ID(s_TpHash(std::chrono::system_clock::now()))
{}

Rxuid::Rxuid(std::string const& s)
	: m_ID(s_StrHash(s))
{}
