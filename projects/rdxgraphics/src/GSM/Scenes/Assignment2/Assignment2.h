#pragma once
#include "GSM/BaseScene.h"

class Assignment2 : public BaseScene
{
	RX_SCENE_DEFAULT(Assignment2);
public:
	void StartImpl() override;
	void UpdateImpl(float dt) override;

private:
	entt::entity m_Spinny{};
};
