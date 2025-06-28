#include "BVHPass.h"
#include "ECS/EntityManager.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Components.h"
#include "Graphics/Object.h"
#include "ECS/Systems/BVHSystem.h"

static glm::vec4 GetLayerColor(int layer)
{
	constexpr float inv255 = 1.f / 255.f;
	constexpr glm::vec4 colors[]{
		glm::vec4{1.f, 0.f, 0.f, 1.f},
		glm::vec4{0.f, 1.f, 0.f, 1.f},
		glm::vec4{1.f, 0.7f, 0.f, 1.f},
		glm::vec4{1.f, 1.f, 0.f, 1.f},
		glm::vec4{1.f, 0.f, 1.f, 1.f},
		glm::vec4{0.f, 1.f, 1.f, 1.f},
		glm::vec4{0.5f, 0.65f, 1.f, 1.f},
	};
	constexpr int colorsLen = 7;

	return colors[layer % colorsLen];
}

static void DrawBVH(std::unique_ptr<BVHNode>& pNode, int layer)
{
	if (!pNode)
		return;

	if (BVHSystem::GetDrawLayers() & (0x1 << layer))
	{
		switch (BVHSystem::GetGlobalBVType())
		{
		case BV::AABB:
		{
			AABBBV& bv = EntityManager::GetComponent<AABBBV>(pNode->Handle);
			auto& obj = RenderSystem::GetObjekt(Shape::Cube);
			obj.Submit<VertexBasic::Xform>(bv.GetXform());
			obj.Submit<VertexBasic::Color>(GetLayerColor(layer));
			break;
		}
		case BV::Sphere:
		{
			SphereBV& bv = EntityManager::GetComponent<SphereBV>(pNode->Handle);
			auto& obj = RenderSystem::GetObjekt(Shape::Sphere);
			obj.Submit<VertexBasic::Xform>(bv.GetXform());
			obj.Submit<VertexBasic::Color>(GetLayerColor(layer));
			break;
		}
		default:
			break;
		}
	}

	if (!pNode->IsLeaf())
	{
		// Recurse
		++layer;
		DrawBVH(pNode->Left, layer);
		DrawBVH(pNode->Right, layer);
	}
}

void BVHPass::DrawImpl() const
{
	auto& pRoot = BVHSystem::GetRootNode();
	if (!pRoot)
		return;

	DrawBVH(pRoot, 0);

	RenderSystem::GetInstance().m_Shader.Bind();
	RenderSystem::GetInstance().m_Shader.SetUniform1i("uIsWireframe", 1);

	glDisable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (auto& [uid, object] : RenderSystem::GetObjekts())
	{
		object.Bind();

		auto& data1 = object.GetVBData<VertexBasic::Xform>();
		size_t maxVal = data1.size(); // glm::min(data1, data2, ...)
		for (size_t count{ 0 }, offset{ 0 }; offset < maxVal; offset += count)
		{
			count = glm::min<size_t>(maxVal - offset, RX_MAX_INSTANCES);
			object.BindInstancedData<VertexBasic::Xform>(offset, count);
			object.BindInstancedData<VertexBasic::Color>(offset, count);

			object.Draw(count);
		}

		object.Flush();
	}
}