#pragma once

class GUI : public BaseSingleton<GUI>
{
	RX_SINGLETON_DECLARATION(GUI);
public:
	static bool Init();
	static void Terminate();
	static void Update(double dt);
	static void Draw();

private:

};