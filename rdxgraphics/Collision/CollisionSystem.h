#pragma once

class CollisionSystem : public BaseSingleton<CollisionSystem>
{
	RX_SINGLETON_DECLARATION(CollisionSystem);
public:
	static void Update(float dt);

private:
};