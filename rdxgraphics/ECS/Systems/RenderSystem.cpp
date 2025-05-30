#include <pch.h>
#include "RenderSystem.h"
#include "GLFWWindow/GLFWWindow.h"
#include "ECS/Components.h"
#include "Graphics/ObjectFactory.h"

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

bool hasDefault = true;
bool hasWireframe = true;
bool hasMinimap = true;

//GLuint m_FBO{};
//GLuint textureColorBuffer{};
RenderPass basePass{};
RenderPass minimapPass{};
RenderPass wireframePass{};
//RenderPass finalPass{};
struct CameraUniform {
	glm::mat4 ViewMatrix{};
	glm::mat4 ProjMatrix{};
	glm::vec4 Position{};
	glm::vec4 Direction{}; // Normalized
	glm::vec2 Clip{};
	glm::vec2 ClipPadding{};
};
UniformBuffer<CameraUniform> testUBO{};

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
	basePass.Init(0, 0, dims.x, dims.y);
	minimapPass.Init(dims.x - 400, dims.y - 400, 400, 400);
	wireframePass.Init(0, 0, dims.x, dims.y);
	//g.m_ScreenPass.Init(nullptr);
	g.m_ScreenPass.Init(0, 0, dims.x, dims.y);

	{
		testUBO.Init(2);
		testUBO.BindBuffer(2);
	}

	return true;
}

void RenderSystem::Terminate()
{
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
		CameraUniform u[2]{
			{
				.ViewMatrix	= activeCamera.GetViewMatrix(),
				.ProjMatrix	= activeCamera.GetProjMatrix(),
				.Position	= { activeCamera.GetPosition(), 0.f},
				.Direction	= { activeCamera.GetDirection(), 1.f},
				.Clip		= activeCamera.GetClipPlanes(),
			},
			{
				.ViewMatrix	= minimapCamera.GetViewMatrix(),
				.ProjMatrix	= minimapCamera.GetProjMatrix(),
				.Position	= { minimapCamera.GetPosition(), 0.f},
				.Direction	= { minimapCamera.GetDirection(), 1.f},
				.Clip		= minimapCamera.GetClipPlanes(),
			} 
		};
		testUBO.Submit(2, u);
	}

	// Inefficient preprocessing of all materials.
	// A necessity dynamicism only in editor because of need for live view of changes made.
	// In game, it's totally fine (probably) to be statically loaded.
	//std::vector<glm::mat4> materials{};
	//{
	//	auto view = EntityManager::View<const Material>();
	//	for (auto [handle, mat] : view.each())
	//	{
	//
	//	}
	//}

	auto BaseAndMinimap =
		[&](bool condition, int camera)
		{
			if (!condition)
				return;

			auto view = EntityManager::View<const Xform, const Model>();
			for (auto [handle, xform, model] : view.each())
			{
				Rxuid meshID = model.GetMesh();
				if (meshID == RX_INVALID_ID)
					continue;

				Object<VertexBasic>& o = GetObjekt(meshID);
				o.Submit<VertexBasic::Xform>(xform.GetXform());

				if (EntityManager::HasComponent<const Material>(handle))
				{
					Material const& mat = EntityManager::GetComponent<const Material>(handle);
					o.Submit<VertexBasic::Color>(static_cast<glm::vec4>(mat));
				}
				else
				{
					o.Submit<VertexBasic::Color>(glm::vec4{1.f, 0.063f, 0.941f, 1.f});
				}
			}

			////glClearColor(g.m_BackColor.x, g.m_BackColor.y, g.m_BackColor.z, 0.f);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			g.m_Shader.Bind();
			g.m_Shader.SetUniform1i("uIsWireframe", 0);
			g.m_Shader.SetUniform1i("uCam", camera);
			g.m_Shader.SetUniform4f("uAmbientLight", g.m_GlobalIllumination);

			{ // directional light hardcode
				auto view = EntityManager::View<DirectionalLight>();
				for (auto [handle, light] : view.each()) // asummed to be 1. it's hardcode so wtv
				{
					g.m_Shader.SetUniform3f("uDirectionalLight", light.GetDirection());
				}
			}

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			for (auto& [uid, object] : g.m_Objects)
			{
				object.Bind();

				auto& data1 = object.GetVBData<VertexBasic::Xform>();
				size_t maxVal = data1.size(); // glm::min(data1, data2, ...)
				for (size_t count{ 0 }, offset{ 0 }; offset < maxVal; offset += count)
				{
					count = glm::min<size_t>(maxVal - offset, RX_MAX_INSTANCES);
					object.BindInstancedData<VertexBasic::Xform>(offset, count);
					object.BindInstancedData<VertexBasic::IsCollide>(offset, count);
					object.BindInstancedData<VertexBasic::Color>(offset, count);
					// more binds...
					object.Draw(count);
				}

				object.Flush();
			}
		};

	basePass.DrawThis(std::bind(BaseAndMinimap, hasDefault, 0));
	minimapPass.DrawThis(std::bind(BaseAndMinimap, hasMinimap, 1));

	wireframePass.DrawThis(
		[&]()
		{
			if (!hasWireframe)
				return;

			//for (Entity& ent : EntityManager::GetEntities())
			//{
			//	auto& colDetails = ent.GetColliderDetails();
			//	if (colDetails.BVType == Primitive::NIL || !colDetails.pBV)
			//		continue;
			//
			//	Object<VertexBasic>& o = GetObjekt(ent.GetColliderDetails().BVType);
			//	o.Submit<VertexBasic::Xform>(colDetails.pBV->GetXform());
			//	o.Submit<VertexBasic::IsCollide>(
			//		(typename VertexBasic::IsCollide::value_type)colDetails.pBV->IsCollide());
			//}
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

					Object<VertexBasic>& o = GetObjekt(Shape::Line);
#define _RX_X(p, t)																	\
					{																\
						glm::mat4 translate = glm::translate(p);					\
						glm::mat4 scale = glm::scale(glm::vec3(glm::length(t)));	\
						glm::mat4 rotate = glm::mat4_cast(glm::rotation(from, glm::normalize(t)));	\
						o.Submit<VertexBasic::Xform>(translate * scale * rotate);	\
						o.Submit<VertexBasic::IsCollide>(prim.IsCollide());			\
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
					/*Should check ensure that get<BV> exists*/						\
					Klass##Primitive& prim = EntityManager::GetComponent<Klass##Primitive>(handle);	\
					Object<VertexBasic>& o = GetObjekt(primType);					\
					o.Submit<VertexBasic::Xform>(prim.GetXform());					\
					o.Submit<VertexBasic::IsCollide>(prim.IsCollide());				\
				}
				RX_DO_ALL_PRIMITIVE_ENUM;
#undef _RX_X
			}

			// Extra per collider type stuff
			auto triangleView = EntityManager::View<TrianglePrimitive>();
			for (auto [handle, bv] : triangleView.each())
			{
				{
					auto& obj = GetObjekt(Primitive::Point);
					//obj.Submit<VertexBasic::Xform>(glm::translate(bv.GetP0_W()));
					//obj.Submit<VertexBasic::Xform>(glm::translate(bv.GetP1_W()));
					//obj.Submit<VertexBasic::Xform>(glm::translate(bv.GetP2_W()));
					obj.Submit<VertexBasic::Xform>(glm::translate(bv.GetPosition()));
					obj.Submit<VertexBasic::IsCollide>(false);
				}
				{
					auto& obj = GetObjekt(Primitive::Ray);
					glm::vec3 from = RayPrimitive::DefaultDirection;
					glm::vec3 to = bv.GetNormal();

					glm::quat quat = glm::rotation(from, to);
					obj.Submit<VertexBasic::Xform>(glm::translate(bv.GetPosition()) * glm::mat4_cast(quat));
					obj.Submit<VertexBasic::IsCollide>(false);
				}
			}

			auto planeView = EntityManager::View<PlanePrimitive>();
			for (auto [handle, bv] : planeView.each())
			{
				{
					auto& obj = GetObjekt(Primitive::Point);
					obj.Submit<VertexBasic::Xform>(glm::translate(bv.GetPosition()));
				}
				{
					auto& obj = GetObjekt(Primitive::Ray);
					glm::vec3 from = RayPrimitive::DefaultDirection;
					glm::vec3 to = bv.GetNormal();

					glm::quat quat = glm::rotation(from, to);
					obj.Submit<VertexBasic::Xform>(glm::translate(bv.GetPosition()) * glm::mat4_cast(quat));
					obj.Submit<VertexBasic::IsCollide>(false);
				}
			}

			{
				auto bvView = EntityManager::View<AABBBV>();
				for (auto [handle, bv] : bvView.each())
				{
					auto& obj = GetObjekt(Shape::Cube);
					obj.Submit<VertexBasic::Xform>(bv.GetXform());
					obj.Submit<VertexBasic::IsCollide>(false);
				}
			}

			//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT);
			//glDisable(GL_DEPTH_TEST); // (optional, just for debugging visibility)
			//glDisable(GL_BLEND);

			//glClearColor(g.m_BackColor.x, g.m_BackColor.y, g.m_BackColor.z, 0.f);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			g.m_Shader.Bind();
			g.m_Shader.SetUniform1i("uIsWireframe", 1);
			g.m_Shader.SetUniform1i("uCam", 0);

			// First pass: Draw actual filled mesh
			glDisable(GL_CULL_FACE);
			//glCullFace(GL_BACK);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			for (auto&[uid, object] : g.m_Objects)
			{
				object.Bind();

				auto& data1 = object.GetVBData<VertexBasic::Xform>();
				size_t maxVal = data1.size(); // glm::min(data1, data2, ...)
				for (size_t count{ 0 }, offset{ 0 }; offset < maxVal; offset += count)
				{
					count = glm::min<size_t>(maxVal - offset, RX_MAX_INSTANCES);
					object.BindInstancedData<VertexBasic::Xform>(offset, count);
					object.BindInstancedData<VertexBasic::IsCollide>(offset, count);
					// more binds...

					if (object.GetPrimitive() == GL_POINTS)
						glPointSize(10.f);

					object.Draw(count);
					glPointSize(1.f);
				}

				object.Flush();
			}
		}
	);

	g.m_ScreenPass.DrawThis(
		[&]()
		{
			glClearColor(g.m_BackColor.x, g.m_BackColor.y, g.m_BackColor.z, 0.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			g.m_FBOShader.Bind();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, basePass.GetTextureBuffer());
			g.m_FBOShader.SetUniform1i("uBaseTex", 0);
			g.m_FBOShader.SetUniform1i("uHasBaseTex", hasDefault);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, wireframePass.GetTextureBuffer());
			g.m_FBOShader.SetUniform1i("uWireframeTex", 1);
			g.m_FBOShader.SetUniform1i("uHasWireframeTex", hasWireframe);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, minimapPass.GetTextureBuffer());
			g.m_FBOShader.SetUniform1i("uMinimapTex", 2);
			g.m_FBOShader.SetUniform1i("uHasMinimapTex", hasMinimap);

			g.m_FBOObject.Bind();
			g.m_FBOObject.Draw(1);
		}
	);

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

#define _RX_X(Klass) GetObjekt(Shape::Klass) = ObjectFactory::CreateObjekt<VertexBasic, ObjectParams_VertexBasic>(ObjectFactory::Setup##Klass());
	_RX_X(Point);
	_RX_X(Line);
	_RX_X(Triangle);
	_RX_X(Quad);
	_RX_X(Plane);
	_RX_X(Cube);
	_RX_X(Sphere);
#undef _RX_X

#define _RX_X(obj)																															\
	{																																		\
	auto obj = ObjectFactory::CreateObjekt<VertexBasic, ObjectParams_VertexBasic>(ObjectFactory::LoadModelFile(RX_MODEL_PREFIX#obj".obj"));	\
		g.m_Objects[Rxuid{ #obj }] = std::move(obj);																						\
	}

	_RX_X(ogre);
	_RX_X(bunny);
	//_RX_X(bunny_high_poly);
	//_RX_X(cup);
	//_RX_X(head);
	//_RX_X(starwars1);
#undef _RX_X
}
