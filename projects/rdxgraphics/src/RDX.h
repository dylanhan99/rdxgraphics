#pragma once

class RDX : public BaseSingleton<RDX>
{
	RX_SINGLETON_DECLARATION(RDX);
public:
	static void Run();
};