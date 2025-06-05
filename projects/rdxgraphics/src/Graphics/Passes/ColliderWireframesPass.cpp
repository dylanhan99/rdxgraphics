#include "ColliderWireframesPass.h"
#include "ECS/EntityManager.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Components.h"
#include "Graphics/Object.h"

void ColliderWireframesPass::DrawImpl() const
{
	auto view = EntityManager::View<Collider>();
	for (auto [handle, collider] : view.each())
	{
		Primitive primType = collider.GetPrimitiveType();
		if (primType == Primitive::NIL)
			continue;

		// Some triangle non-conforming hardcode.
		// Hopefully is temporary. I'd prefer to use the actual
		// triangle mesh.
		if (primType == Primitive::Triangle)
		{
			TrianglePrimitive& prim = EntityManager::GetComponent<TrianglePrimitive>(handle);
			glm::vec3 from = RayPrimitive::DefaultDirection;

			glm::vec3 p0 = prim.GetP0_W();
			glm::vec3 p1 = prim.GetP1_W();
			glm::vec3 p2 = prim.GetP2_W();

			glm::vec3 to0 = p1 - p0;
			glm::vec3 to1 = p2 - p1;
			glm::vec3 to2 = p0 - p2;

			Object<VertexBasic>& o = RenderSystem::GetObjekt(Shape::Line);
#define _RX_X(p, t)																	\
					{																\
						glm::mat4 translate = glm::translate(p);					\
						glm::mat4 scale = glm::scale(glm::vec3(glm::length(t)));	\
						glm::mat4 rotate = glm::mat4_cast(glm::rotation(from, glm::normalize(t)));	\
						o.Submit<VertexBasic::Xform>(translate * scale * rotate);	\
						o.Submit<VertexBasic::Color>(prim.IsCollide() ? glm::vec4{1.f,0.f,0.f,1.f} : glm::vec4{0.f,1.f,0.f,1.f});\
					}
			_RX_X(p0, to0);
			_RX_X(p1, to1);
			_RX_X(p2, to2);
#undef _RX_X
			continue;
		}

#define _RX_X(Klass)																\
				if (primType == Primitive::Klass)									\
				{																	\
					Klass##Primitive& prim = EntityManager::GetComponent<Klass##Primitive>(handle);	\
					Object<VertexBasic>& o = RenderSystem::GetObjekt(primType);		\
					o.Submit<VertexBasic::Xform>(prim.GetXform());					\
					o.Submit<VertexBasic::Color>(prim.IsCollide() ? glm::vec4{1.f,0.f,0.f,1.f} : glm::vec4{0.f,1.f,0.f,1.f});\
				}
		RX_DO_ALL_PRIMITIVE_ENUM;
#undef _RX_X
	}

	// Extra per collider type stuff
	auto triangleView = EntityManager::View<TrianglePrimitive>();
	for (auto [handle, prim] : triangleView.each())
	{
		{
			auto& obj = RenderSystem::GetObjekt(Primitive::Point);
			obj.Submit<VertexBasic::Xform>(glm::translate(prim.GetPosition()));
			obj.Submit<VertexBasic::Color>(glm::vec4{ 0.f,1.f,0.f,1.f });
		}
		{
			auto& obj = RenderSystem::GetObjekt(Primitive::Ray);
			glm::vec3 from = RayPrimitive::DefaultDirection;
			glm::vec3 to = prim.GetNormal();

			glm::quat quat = glm::rotation(from, to);
			obj.Submit<VertexBasic::Xform>(glm::translate(prim.GetPosition()) * glm::mat4_cast(quat));
			obj.Submit<VertexBasic::Color>(glm::vec4{ 0.f,1.f,0.f,1.f });
		}
	}

	auto planeView = EntityManager::View<PlanePrimitive>();
	for (auto [handle, prim] : planeView.each())
	{
		{
			auto& obj = RenderSystem::GetObjekt(Primitive::Point);
			obj.Submit<VertexBasic::Xform>(glm::translate(prim.GetPosition()));
		}
		{
			auto& obj = RenderSystem::GetObjekt(Primitive::Ray);
			glm::vec3 from = RayPrimitive::DefaultDirection;
			glm::vec3 to = prim.GetNormal();

			glm::quat quat = glm::rotation(from, to);
			obj.Submit<VertexBasic::Xform>(glm::translate(prim.GetPosition()) * glm::mat4_cast(quat));
			obj.Submit<VertexBasic::Color>(glm::vec4{ 0.f,1.f,0.f,1.f });
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
