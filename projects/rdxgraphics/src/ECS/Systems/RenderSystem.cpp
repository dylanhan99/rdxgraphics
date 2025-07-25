#include "RenderSystem.h"
#include "GLFWWindow/GLFWWindow.h"
#include "ECS/Components.h"
#include "Graphics/ObjectFactory.h"
#include "Graphics/Passes/Passes.h"

#if USE_CSD3151_AUTOMATION == 1
// This automation hook reads the shader from the submission tutorial's shader directory as a string literal.
// It requires an automation script to convert the shader files from file format to string literal format.
// After conversion, the file names must be changed to my-shader.vert and my-shader.frag.
std::string const assignment_vs = {
  #include "../shaders/default.vert.h"
};
std::string const assignment_fs = {
  #include "../shaders/default.frag.h"
};
std::string const fbo_vs = {
  #include "../shaders/screen.vert.h"
};
std::string const fbo_fs = {
  #include "../shaders/screen.frag.h"
};
#endif

RX_SINGLETON_EXPLICIT(RenderSystem);
namespace fs = std::filesystem;

bool RenderSystem::Init()
{
	GLenum succ = glewInit();
	RX_INFO("{}", succ);
	//if (!gladLoadGL(glfwGetProcAddress)) 
	//{
	//	RX_ERROR("Failed to initialize GLAD");
	//	return false;
	//}

	RX_INFO("GL Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	GLint maxUBOBindings{}; glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUBOBindings);
	RX_INFO("GL max UBO bindings: {}", maxUBOBindings);

	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if USE_CSD3151_AUTOMATION == 1
	g.m_Shader.Init(std::vector<std::pair<ShaderType, std::string>>{
		{ ShaderType::Vertex,	std::string{ assignment_vs } },
		{ ShaderType::Fragment, std::string{ assignment_fs } }
	});
	g.m_FBOShader.Init(std::vector<std::pair<ShaderType, std::string>>{
		{ ShaderType::Vertex,	std::string{ fbo_vs } },
		{ ShaderType::Fragment, std::string{ fbo_fs } }
	});
#else
	g.m_Shader.Init({
		{ ShaderType::Vertex,	fs::path{ RX_SHADER_PREFIX"default.vert" } },
		{ ShaderType::Fragment, fs::path{ RX_SHADER_PREFIX"default.frag" } }
		});
	g.m_FBOShader.Init({
		{ ShaderType::Vertex,	fs::path{ RX_SHADER_PREFIX"screen.vert" } },
		{ ShaderType::Fragment, fs::path{ RX_SHADER_PREFIX"screen.frag" } }
		});
#endif

	CreateShapes();

	EventDispatcher<Camera&>::RegisterEvent(RX_EVENT_CAMERA_USER_TOGGLED,
		[](Camera& camera)
		{
			GLFWWindow::SetInvisibleCursor(camera.IsCameraInUserControl());
		});

	glm::ivec2 dims = GLFWWindow::GetWindowDims();

	// screenpass will foreach the Passes, and automatically slurp according to data provided
	//RegisterPass<Type>("display name", "shader boolean (uHas_)", default_enabled = true_or_false).Init(dims);

	auto pModelsPass = RegisterPass<ModelsPass>("Models & Lighting", "Models", true);
	auto pColliderWireframesPass = RegisterPass<ColliderWireframesPass>("Collider Wireframes", "Colliders", false);
	auto pBVLeafPass = RegisterPass<BVLeafPass>("BV Leafs", "BVLeafs", false);
	auto pBVHPass = RegisterPass<BVHPass>("BVH", "BVH", true);
	auto pPiPModelsPass = RegisterPass<ModelsPass>("PiP Models & Lighting", "PiPModels", true);
	auto pPiPBVLeafPass = RegisterPass<BVLeafPass>("PiP BV Leafs", "PiPBVLeafs", false);
	auto pPiPBVHPass = RegisterPass<BVHPass>("PiP BVH", "PiPBVH", true);

	pModelsPass->Init(0, 0, dims.x, dims.y);
	pColliderWireframesPass->Init(0, 0, dims.x, dims.y);
	pBVLeafPass->Init(0, 0, dims.x, dims.y);
	pBVHPass->Init(0, 0, dims.x, dims.y);
	pPiPModelsPass->Init(dims.x - 400, dims.y - 400, 400, 400);
	pPiPBVLeafPass->Init(dims.x - 400, dims.y - 400, 400, 400);
	pPiPBVHPass->Init(dims.x - 400, dims.y - 400, 400, 400);

	// Must be the final pass
	RegisterPass<ScreenPass>("Screen", "Screen")->Init(0, 0, dims.x, dims.y);


#define _RX_X(name) std::bind(static_cast<void(BaseUniformBuffer::*)()const>(&BaseUniformBuffer::BindBuffer), GetUBO(name))
	RegisterUBO<ShaderUniform::Camera>("ActiveCam", 1, 2);
	RegisterUBO<ShaderUniform::Camera>("PiPCam", 1, 2);

	pModelsPass->RegisterUBOBind(_RX_X("ActiveCam"));
	pColliderWireframesPass->RegisterUBOBind(_RX_X("ActiveCam"));
	pBVLeafPass->RegisterUBOBind(_RX_X("ActiveCam"));
	pBVHPass->RegisterUBOBind(_RX_X("ActiveCam"));
	pPiPModelsPass->RegisterUBOBind(_RX_X("PiPCam"));
	pPiPBVLeafPass->RegisterUBOBind(_RX_X("PiPCam"));
	pPiPBVHPass->RegisterUBOBind(_RX_X("PiPCam"));
#undef _RX_X

	return true;
}

void RenderSystem::Terminate()
{
	for (auto& ubo : g.m_UBOs)
		ubo.second->Terminate();

	for (auto& [uid, object] : g.m_Objects)
		object.Terminate();

	g.m_Shader.Terminate();
}

void RenderSystem::Draw()
{
	entt::entity const camEnt = GetActiveCamera();
	entt::entity const miniEnt = GetMinimapCamera();
	// Could just NOT draw anything i suppose?
	RX_ASSERT(EntityManager::HasComponent<Camera>(camEnt), "Active camera entity is missing Camera component");
	Camera& activeCamera = EntityManager::GetComponent<Camera>(camEnt);
	Camera& minimapCamera = EntityManager::GetComponent<Camera>(miniEnt);

	{
		ShaderUniform::Camera activeCam
		{
			.ViewMatrix = activeCamera.GetViewMatrix(),
			.ProjMatrix = activeCamera.GetProjMatrix(),
			.Position = { activeCamera.GetPosition(), 0.f},
			.Direction = { activeCamera.GetDirection(), 1.f},
			.Clip = activeCamera.GetClipPlanes(),
		};
		GetUBO("ActiveCam")->Submit(1, &activeCam);

		ShaderUniform::Camera pipCam
		{
			.ViewMatrix = minimapCamera.GetViewMatrix(),
			.ProjMatrix = minimapCamera.GetProjMatrix(),
			.Position = { minimapCamera.GetPosition(), 0.f},
			.Direction = { minimapCamera.GetDirection(), 1.f},
			.Clip = minimapCamera.GetClipPlanes(),
		};
		GetUBO("PiPCam")->Submit(1, &pipCam);
	}

	for (auto& pass : g.m_RenderPasses)
		pass->Draw();

	glBindVertexArray(0);
}

bool RenderSystem::ReloadShaders()
{
	return g.m_Shader.Reload();
}

Object<VertexBasic>& RenderSystem::GetObjekt(Rxuid uid)
{
	RX_ASSERT(uid != RX_INVALID_ID);
	return g.m_Objects[uid];
}

Object<VertexBasic>& RenderSystem::GetObjekt(Shape shape)
{
	return g.m_Objects[Rxuid{ shape }];
}

Object<VertexBasic>& RenderSystem::GetObjekt(Primitive bv)
{
	switch (bv)
	{
	case Primitive::Point:
		return GetObjekt(Shape::Point);
	case Primitive::Ray:
		return GetObjekt(Shape::Line);
	case Primitive::Triangle:
		return GetObjekt(Shape::Triangle);
	case Primitive::Plane:
		return GetObjekt(Shape::Plane);
	case Primitive::AABB:
		return GetObjekt(Shape::Cube);
	case Primitive::Sphere:
		return GetObjekt(Shape::Sphere);
	default:
		RX_ASSERT(false);
		return GetObjekt(Shape::Cube);
	}
}

void RenderSystem::CreateShapes()
{
	g.m_FBOObject = std::move(ObjectFactory::CreateObjekt<VertexFBO, ObjectParams_VertexFBO>(ObjectFactory::CreateScreenQuad()));

	auto CreateShape =
		[&](Rxuid id, std::string name, Object<VertexBasic>&& obj)
		{
			obj.SetName(name);
			g.m_Objects[id] = std::move(obj);																					\
		};

#define _RX_X(Klass)													\
	CreateShape(Rxuid(Shape::Klass), #Klass,							\
	ObjectFactory::CreateObjekt<VertexBasic, ObjectParams_VertexBasic>(	\
		ObjectFactory::Setup##Klass()));

	RX_DO_ALL_SHAPE_ENUM;
#undef _RX_X

#define _RX_XX(name, path)													\
	CreateShape(Rxuid{ path }, name,										\
		ObjectFactory::CreateObjekt<VertexBasic, ObjectParams_VertexBasic>(	\
			ObjectFactory::LoadModelFile(RX_MODEL_PREFIX##path".obj")));
#define _RX_X(obj) _RX_XX(#obj, #obj)

	RX_DO_ALL_OBJ;
	RX_DO_ALL_UNC_M(_RX_XX);

#undef _RX_XX
#undef _RX_X
}
