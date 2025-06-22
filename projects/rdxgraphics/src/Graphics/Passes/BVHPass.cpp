#include "BVHPass.h"
#include "ECS/EntityManager.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Components.h"
#include "Graphics/Object.h"
#include "ECS/Systems/BVHSystem.h"

static void DrawBVH(std::unique_ptr<BVHNode>& pNode, int layer)
{
	if (!pNode)
		return;

	// Draw shape
	AABBBV& bv = EntityManager::GetComponent<AABBBV>(pNode->Handle);
	auto& obj = RenderSystem::GetObjekt(Shape::Cube);
	obj.Submit<VertexBasic::Xform>(bv.GetXform());
	obj.Submit<VertexBasic::Color>(glm::vec4{0.f,1.f,0.f,1.f});// GetBVColor(bv.GetBVState()));

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