#pragma once
#include "GSM/BaseScene.h"

class Assignment3 : public BaseScene
{
	RX_SCENE_DEFAULT(Assignment3);
public:
	void StartImpl() override;
	void UpdateImpl(float dt) override;
};
