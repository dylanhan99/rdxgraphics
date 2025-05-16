#pragma once
#include "GSM/BaseScene.h"

class CommonLayer : public BaseScene
{
	RX_SCENE_DEFAULT(CommonLayer);
public:
	void Load() override;
	void Start() override;
	void Update(float dt) override;

private:
	entt::entity m_ActiveCamera{};

	entt::entity m_MainCamera{};
	entt::entity m_MinimapCamera{};
};