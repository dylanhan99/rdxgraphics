#pragma once
#include "Graphics/GraphicsCommon.h"
#include "Graphics/Object.h"
#include "Graphics/Shader.h"
#include "Graphics/RenderPass.h"
#include "ECS/EntityManager.h"

class RenderSystem : public BaseSingleton<RenderSystem>
{
	RX_SINGLETON_DECLARATION(RenderSystem);
private:
#define _RX_X(Klass, ...) typename Klass::container_type,
	using ObjectParams = std::tuple<Shape, GLenum, std::vector<GLuint>, 
		RX_VERTEX_BASIC_ATTRIBS_M_NOINSTANCED(_RX_X)
		void* // Hack to overcome the trailing comma
	>;
#undef _RX_X

public:
	static bool Init();
	static void Terminate();
	static void Draw();

	inline static glm::vec3& GetBackBufferColor() { return g.m_BackColor; }

	static bool ReloadShaders();

	// Helpful for degbug drawing
	template <Shape S, typename U>
	static void Submit(typename U::value_type val) { GetObjekt(S).Submit<U>(val); }

	static Object<VertexBasic>& GetObjekt(Rxuid uid);
	static Object<VertexBasic>& GetObjekt(Shape shape);
	static Object<VertexBasic>& GetObjekt(BV bv);

	inline static entt::entity GetActiveCamera() { return g.m_ActiveCamera; }
	inline static void SetActiveCamera(entt::entity handle) { g.m_ActiveCamera = handle; }

	inline static entt::entity GetMinimapCamera() { return g.m_MinimapCamera; }
	inline static void SetMinimapCamera(entt::entity handle) { g.m_MinimapCamera = handle; }

	inline static RenderPass& GetScreenPass() { return g.m_ScreenPass; }

private:
	static void CreateShapes();

	static void CreateObjekt(ObjectParams const& objParams);
	static ObjectParams CreatePoint();
	static ObjectParams CreateLine();
	static ObjectParams CreateQuad();
	static ObjectParams CreatePlane();
	static ObjectParams CreateCube();
	static ObjectParams CreateSphere(int refinement = 3);

private:
	glm::vec3 m_BackColor{ 0.2f, 0.3f, 0.3f };
	Shader m_Shader{};
	Shader m_FBOShader{};

	//std::array<Object<VertexBasic>, (size_t)Shape::MAX> m_Objects{};
	std::map<Rxuid, Object<VertexBasic>> m_Objects{};
	Object<VertexFBO> m_FBOObject{};

	entt::entity m_ActiveCamera{};
	entt::entity m_MinimapCamera{};

	RenderPass m_ScreenPass{};
};
