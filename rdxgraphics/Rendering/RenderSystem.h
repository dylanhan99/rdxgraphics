#pragma once
#include "Object.h"
#include "Shader.h"

class RenderSystem : public BaseSingleton<RenderSystem>
{
	RX_SINGLETON_DECLARATION(RenderSystem);
public:
	static bool Init();
	static void Terminate();

	static void Update(double dt);

	inline static glm::vec3& GetBackBufferColor() { return g.m_BackColor; }

	static bool ReloadShaders();

	inline static Object& GetObjekt(Shape shape) { return g.m_Objects[(size_t)shape]; }

private:
	static void CreateShapes();

private:
	glm::vec3 m_BackColor{ 0.2f, 0.3f, 0.3f };
	//GLuint m_ShaderProgramID{};
	Shader m_Shader{};

	std::array<Object, (size_t)Shape::MAX> m_Objects{};
};
