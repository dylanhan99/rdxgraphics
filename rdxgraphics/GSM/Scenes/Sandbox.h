#pragma once
#include "GSM/BaseScene.h"

class Sandbox : public BaseScene
{
	RX_SCENE_DEFAULT(Sandbox);
public:
	void Load() override;
	void Start() override;
	void Update(float dt) override;
};