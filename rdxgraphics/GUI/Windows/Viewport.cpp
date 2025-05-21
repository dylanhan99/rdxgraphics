#include <pch.h>
#include "Viewport.h"
#include "ECS/Systems/RenderSystem.h"

void Viewport::UpdateImpl(float dt)
{
	auto& pass = RenderSystem::GetScreenPass();
	ImVec2 winDims{
		ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x,
		ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y
	};
	glm::vec2 bufDims = pass.GetBufferDims();
	float ar = bufDims.x / bufDims.y;
	glm::vec2 vpSize{};

	{ // restrict size based on imgui window height. 
		// larger ar == wider window == base on height
		if ((winDims.x / winDims.y) > ar)
		{
			vpSize = {
				winDims.y * ar,
				winDims.y
			};
		}
		// smaller ar == shorter window == base on wdth
		else
		{
			vpSize = {
				winDims.x,
				winDims.x / ar
			};
		}
	}

	//{
	//	glm::vec2 sizeA{
	//		winDims.y * ar,
	//		winDims.y
	//	};
	//	glm::vec2 sizeB{
	//		winDims.x,
	//		winDims.x / ar
	//	};
	//
	//	vpSize = (sizeA.x * sizeA.y > sizeB.x * sizeB.y) ? sizeA : sizeB;
	//}

	GLuint const frameHandle = pass.GetTextureBuffer();
	ImGui::Image(frameHandle, 
		ImVec2{ vpSize.x, vpSize.y },
		{ 0.f, 1.f }, { 1.f, 0.f }); // It's flipped vertically
}