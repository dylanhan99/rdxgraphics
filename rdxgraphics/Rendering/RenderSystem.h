#pragma once
#include "Object.h"
#include "Shader.h"
#include "RenderPass.h"

class RenderSystem : public BaseSingleton<RenderSystem>
{
	RX_SINGLETON_DECLARATION(RenderSystem);
public:
	static bool Init();
	static void Terminate();

	static void Update(double dt);

	inline static glm::vec3& GetBackBufferColor() { return g.m_BackColor; }

	static bool ReloadShaders();

	inline static Object<VertexBasic>& GetObjekt(Shape shape) { return g.m_Objects[(size_t)shape]; }
	static Object<VertexBasic>& GetObjekt(BV bv);

private:
	static void CreateShapes();

private:
	glm::vec3 m_BackColor{ 0.2f, 0.3f, 0.3f };
	Shader m_Shader{};
	Shader m_FBOShader{};

	std::array<Object<VertexBasic>, (size_t)Shape::MAX> m_Objects{};
	Object<VertexFBO> m_FBOObject{};
};
