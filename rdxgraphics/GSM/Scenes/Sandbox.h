#pragma once
#include "GSM/BaseScene.h"

class Sandbox : public BaseScene
{
public:
	void Load() override;
	void Start() override;
	void Update(float dt) override;
};