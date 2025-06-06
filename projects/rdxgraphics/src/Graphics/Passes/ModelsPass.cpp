#include "ModelsPass.h"
#include "ECS/EntityManager.h"
#include "ECS/Systems/RenderSystem.h"

void ModelsPass::DrawImpl() const
{
	auto view = EntityManager::View<const Xform, const Model>();
	for (auto [handle, xform, model] : view.each())
	{
		// If has BV, then bother to cull or not.
		// If no BV, just draw no problem
		if (EntityManager::HasComponent<BoundingVolume>(handle))
		{
#define _RX_X(Klass) case BV::Klass:									  \
			{															  \
				auto& bv = EntityManager::GetComponent<Klass##BV>(handle);\
				if (bv.GetBVState() == BVState::Out) continue;			  \
				break; 													  \
			}

			BoundingVolume& boundingVolume = EntityManager::GetComponent<BoundingVolume>(handle);
			switch (boundingVolume.GetBVType())
			{
				RX_DO_ALL_BV_ENUM;
			default: break; // NIL is also considered as "no BV"
			}
#undef _RX_X
		}

		Rxuid meshID = model.GetMesh();
		if (meshID == RX_INVALID_ID)
			continue;

		Object<VertexBasic>& o = RenderSystem::GetObjekt(meshID);
		o.Submit<VertexBasic::Xform>(xform.GetXform());

		if (EntityManager::HasComponent<const Material>(handle))
		{
			Material const& mat = EntityManager::GetComponent<const Material>(handle);
			o.Submit<VertexBasic::Color>(static_cast<glm::vec4>(mat));
		}
		else
		{
			o.Submit<VertexBasic::Color>(glm::vec4{ 1.f, 0.063f, 0.941f, 1.f });
		}
	}

	RenderSystem::GetInstance().m_Shader.Bind();
	RenderSystem::GetInstance().m_Shader.SetUniform1i("uIsWireframe", 0);
	RenderSystem::GetInstance().m_Shader.SetUniform4f("uAmbientLight", RenderSystem::GetGlobalIllumination());

	{ // directional light hardcode
		auto view = EntityManager::View<DirectionalLight>();
		for (auto [handle, light] : view.each()) // asummed to be 1. it's hardcode so wtv
		{
			RenderSystem::GetInstance().m_Shader.SetUniform3f("uDirectionalLight", light.GetDirection());
		}
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
			// more binds...
			object.Draw(count);
		}

		object.Flush();
	}
}