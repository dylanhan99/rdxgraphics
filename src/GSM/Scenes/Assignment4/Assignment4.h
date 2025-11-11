#pragma once
#include "GSM/BaseScene.h"

class Assignment4 : public BaseScene
{
	RX_SCENE_DEFAULT(Assignment4);
public:
	void StartImpl() override;
	void UpdateImpl(float dt) override;
};
