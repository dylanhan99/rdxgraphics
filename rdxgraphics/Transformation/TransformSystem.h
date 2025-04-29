#pragma once

class TransformSystem : public BaseSingleton<TransformSystem>
{
	RX_SINGLETON_DECLARATION(TransformSystem);
public:
	static bool Init();
	static void Terminate();

	static void Update(float dt);

private:

};