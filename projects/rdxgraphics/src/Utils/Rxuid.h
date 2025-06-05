#pragma once

class Rxuid
{
public:
	Rxuid(Rxuid const&) = default;

	Rxuid();
	Rxuid(std::string const& s);
	inline Rxuid(uint64_t v) : m_ID(v) {}
	inline Rxuid(Shape v) : Rxuid((uint64_t)v) {}

	inline std::string ToString() const { return std::to_string(m_ID); }
	inline operator uint64_t() const { return m_ID; }

private:
	uint64_t m_ID{ RX_INVALID_ID };
};

namespace std 
{
	template<> 
	struct hash<Rxuid>
	{
		std::size_t operator()(const Rxuid& uuid) const noexcept
		{
			//return std::hash<uint64_t>()(uuid.GetID());
			return static_cast<uint64_t>(uuid);
		}
	};
}