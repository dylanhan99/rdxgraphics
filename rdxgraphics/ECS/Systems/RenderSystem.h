#pragma once
#include "Graphics/GraphicsCommon.h"
#include "Graphics/Object.h"
#include "Graphics/Shader.h"
#include "Graphics/RenderPass.h"
#include "Graphics/UniformBuffer.h"
#include "ECS/EntityManager.h"

class RenderSystem : public BaseSingleton<RenderSystem>
{
	RX_SINGLETON_DECLARATION(RenderSystem);
public:
	static bool Init();
	static void Terminate();
	static void Draw();

	inline static glm::vec3& GetBackBufferColor() { return g.m_BackColor; }

	static bool ReloadShaders();

	// Helpful for degbug drawing
	template <Shape S, typename U>
	static void Submit(typename U::value_type val) { GetObjekt(S).Submit<U>(val); }

	inline static std::map<Rxuid, Object<VertexBasic>> const& GetObjekts() { return g.m_Objects; }
	static Object<VertexBasic>& GetObjekt(Rxuid uid);
	static Object<VertexBasic>& GetObjekt(Shape shape);
	static Object<VertexBasic>& GetObjekt(Primitive bv);

	inline static entt::entity GetActiveCamera() { return g.m_ActiveCamera; }
	inline static void SetActiveCamera(entt::entity handle) { g.m_ActiveCamera = handle; }

	inline static entt::entity GetMinimapCamera() { return g.m_MinimapCamera; }
	inline static void SetMinimapCamera(entt::entity handle) { g.m_MinimapCamera = handle; }

	inline static RenderPass& GetScreenPass() { return g.m_ScreenPass; }

private:
	static void CreateShapes();

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
