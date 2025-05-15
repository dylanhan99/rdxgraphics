#pragma once

class BaseScene
{
public:
	~BaseScene() = default;

	virtual void Load() = 0;
	inline virtual void Unload(){};
	virtual void Start() = 0;
	virtual void Update(float dt) = 0;

private:
};