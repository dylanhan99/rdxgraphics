#include "BVWireframesPass.h"
#include "ECS/EntityManager.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Components.h"
#include "Graphics/Object.h"

glm::vec4 const InColor{ 0.2f,0.85f,1.f,1.f };
glm::vec4 const OutColor{ 1.f,0.f,0.f,1.f };
glm::vec4 const OnColor{ 1.f,1.f,0.f,1.f };

static glm::vec4 GetBVColor(BVState state)
{
	switch (state)
	{
	case BVState::In:
		return InColor;
	case BVState::Out:
		return OutColor;
	case BVState::On:
		return OnColor;
	}
}

void BVWireframesPass::DrawImpl() const
{
	{
		auto bvView = EntityManager::View<const FrustumBV>();
		auto& obj = RenderSystem::GetObjekt(Shape::Line);
		for (auto [handle, bv] : bvView.each())
		{
			if (handle != RenderSystem::GetActiveCamera())
				continue;

			for (glm::mat4 const& edge : bv.GetEdgeXforms())
			{
				obj.Submit<VertexBasic::Xform>(edge);
				obj.Submit<VertexBasic::Color>(glm::vec4{ 1.f, 0.063f, 0.941f, 1.f });
			}

			// Where is the point? 
			//auto const& points = bv.GetPoints();
			//std::vector<glm::vec3> planePositions{};
			//planePositions.emplace_back(glm::vec3(points[4] + 0.5f * (points[4] - points[6])));
			//planePositions.emplace_back(glm::vec3(points[3] + 0.5f * (points[3] - points[1])));
			//planePositions.emplace_back(glm::vec3(points[0] + 0.5f * (points[0] - points[5])));
			//planePositions.emplace_back(glm::vec3(points[7] + 0.5f * (points[7] - points[2])));
			//planePositions.emplace_back(glm::vec3(points[3] + 0.5f * (points[3] - points[4])));
			//planePositions.emplace_back(glm::vec3(points[1] + 0.5f * (points[1] - points[6])));
		}
	}
	{
		auto bvView = EntityManager::View<AABBBV>();
		for (auto [handle, bv] : bvView.each())
		{
			auto& obj = RenderSystem::GetObjekt(Shape::Cube);
			obj.Submit<VertexBasic::Xform>(bv.GetXform());
			obj.Submit<VertexBasic::Color>(GetBVColor(bv.GetBVState()));
		}
	}
	{
		auto bvView = EntityManager::View<OBBBV>();
		for (auto [handle, bv] : bvView.each())
		{
			auto& obj = RenderSystem::GetObjekt(Shape::Cube);
			obj.Submit<VertexBasic::Xform>(bv.GetXform());
			obj.Submit<VertexBasic::Color>(GetBVColor(bv.GetBVState()));
		}
	}
	{
		auto bvView = EntityManager::View<SphereBV>();
		for (auto [handle, bv] : bvView.each())
		{
			auto& obj = RenderSystem::GetObjekt(Shape::Sphere);
			obj.Submit<VertexBasic::Xform>(bv.GetXform());
			obj.Submit<VertexBasic::Color>(GetBVColor(bv.GetBVState()));
		}
	}

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