#pragma once
#include "Graphics/GraphicsCommon.h"
#include "Graphics/Object.h"
#include "Graphics/Shader.h"
#include "Graphics/UniformBuffer.h"
#include "ECS/EntityManager.h"
#include "Graphics/BasePass.h"

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

	inline static std::map<Rxuid, Object<VertexBasic>>& GetObjekts() { return g.m_Objects; }
	static Object<VertexBasic>& GetObjekt(Rxuid uid);
	static Object<VertexBasic>& GetObjekt(Shape shape);
	static Object<VertexBasic>& GetObjekt(Primitive bv);

	inline static entt::entity GetActiveCamera() { return g.m_ActiveCamera; }
	inline static void SetActiveCamera(entt::entity handle) { g.m_ActiveCamera = handle; }

	inline static entt::entity GetMinimapCamera() { return g.m_MinimapCamera; }
	inline static void SetMinimapCamera(entt::entity handle) { g.m_MinimapCamera = handle; }

	//template <typename T>
	//inline static T& GetRenderPass() { return *std::dynamic_pointer_cast<T>(g.m_RenderPasses[GetPassID<T>()]); }
	inline static std::vector<std::shared_ptr<BasePass>>& GetRenderPasses() { return g.m_RenderPasses; }
	inline static std::shared_ptr<BasePass> GetScreenPass() { return g.m_RenderPasses.back(); } // We assume that screenpass is the final

	// Returns nullptr if not found
	inline static std::shared_ptr<BaseUniformBuffer> GetUBO(std::string const& name) { auto it = g.m_UBOs.find(name); return it != g.m_UBOs.end() ? it->second : nullptr; }

	inline static glm::vec4& GetGlobalIllumination() { return g.m_GlobalIllumination; }

private:
	static void CreateShapes();

	template <typename T, typename ...Args>
	//static std::enable_if<
	//	std::is_base_of_v<BasePass, T> && std::is_constructible_v<T, Args...>, std::unique_ptr<BasePass>&> 
	static std::shared_ptr<BasePass>& RegisterPass(Args&& ...args)
	{
		return g.m_RenderPasses.emplace_back(std::move(std::make_shared<T>(std::forward<Args>(args)...)));
	}

	template <typename T>
	static void RegisterUBO(std::string const& name, size_t count, GLuint slot)
	{
		RX_ASSERT(g.m_UBOs.find(name) == g.m_UBOs.end(), "Cannot have duplicate UBO names");

		std::shared_ptr<UniformBuffer<T>> ubo = std::make_shared<UniformBuffer<T>>();
		ubo->Init(count, slot);
		g.m_UBOs.emplace(name, ubo);
	}

public:
	glm::vec3 m_BackColor{ 0.2f, 0.3f, 0.3f };
	glm::vec4 m_GlobalIllumination{ 1.f,0.0627f,0.941f,0.7f }; // w is the scale factor, [0.f,1.f]
	Shader m_Shader{};
	Shader m_FBOShader{};

	//std::array<Object<VertexBasic>, (size_t)Shape::MAX> m_Objects{};
	std::map<Rxuid, Object<VertexBasic>> m_Objects{};
	Object<VertexFBO> m_FBOObject{};

	entt::entity m_ActiveCamera{};
	entt::entity m_MinimapCamera{};

	// This is rendered in register order.
	std::vector<std::shared_ptr<BasePass>> m_RenderPasses{};
	std::map <std::string, std::shared_ptr<BaseUniformBuffer>> m_UBOs{};
};

namespace ShaderUniform
{
	struct Camera {
		glm::mat4 ViewMatrix{};
		glm::mat4 ProjMatrix{};
		glm::vec4 Position{};
		glm::vec4 Direction{}; // Normalized
		glm::vec2 Clip{};
		glm::vec2 ClipPadding{};
	};


}