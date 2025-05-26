#pragma once
#include "GSM/BaseScene.h"

class CommonLayer : public BaseScene
{
	RX_SCENE_DEFAULT(CommonLayer);
public:
	void StartImpl() override;
	void UpdateImpl(float dt) override;

private:
	entt::entity m_ActiveCamera{};

	entt::entity m_MainCamera{};
	entt::entity m_MinimapCamera{};
	entt::entity m_LightHandle{};
};