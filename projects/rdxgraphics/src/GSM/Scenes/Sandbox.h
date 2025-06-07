#pragma once
#include "GSM/BaseScene.h"

class Sandbox : public BaseScene
{
	RX_SCENE_DEFAULT(Sandbox);
public:
	void StartImpl() override;
	void UpdateImpl(float dt) override;
};