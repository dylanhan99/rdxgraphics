#include <pch.h>
#include "RenderSystem.h"
#include "GLFWWindow/GLFWWindow.h"
#include "ECS/Components/Camera.h"

#if USE_CSD3151_AUTOMATION == 1
// This automation hook reads the shader from the submission tutorial's shader directory as a string literal.
// It requires an automation script to convert the shader files from file format to string literal format.
// After conversion, the file names must be changed to my-shader.vert and my-shader.frag.
std::string const assignment_vs = {
  #include "../shaders/my-shader.vert"
};
std::string const assignment_fs = {
  #include "../shaders/my-shader.frag"
};
std::string const fbo_vs = {
  #include "../shaders/screen.vert"
};
std::string const fbo_fs = {
  #include "../shaders/screen.frag"
};
#endif

RX_SINGLETON_EXPLICIT(RenderSystem);
namespace fs = std::filesystem;

float move = 0.f;
int renderOption = 2;

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
	// Init shaders with shader buffer directly instead.
#else
	g.m_Shader.Init({
		{ ShaderType::Vertex,	RX_SHADER_PREFIX"default.vert" },
		{ ShaderType::Fragment, RX_SHADER_PREFIX"default.frag" }
		});
	g.m_FBOShader.Init({
		{ ShaderType::Vertex,	RX_SHADER_PREFIX"screen.vert" },
		{ ShaderType::Fragment, RX_SHADER_PREFIX"screen.frag" }
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

	basePass.DrawThis(
		[&]()
		{
			if (renderOption == 1)
				return;

			auto view = EntityManager::View<const Xform, const Model>();
			for (auto [handle, xform, model] : view.each())
			{
				Rxuid meshID = model.GetMesh();
				if (meshID == RX_INVALID_ID)
					continue;
			
				Object<VertexBasic>& o = GetObjekt(meshID);
				o.Submit<VertexBasic::Xform>(xform.GetXform());

				// Temporary preprocessing to submit
				if (EntityManager::HasComponent<const Material>(handle))
				{
					Material const& mat = EntityManager::GetComponent<const Material>(handle);
					o.Submit<VertexBasic::MatID>(1.f);
					o.Submit<VertexBasic::Material>(static_cast<glm::mat4>(mat));
				}
				else
				{
					o.Submit<VertexBasic::MatID>(false);
					o.Submit<VertexBasic::Material>(glm::mat4{});
				}
			}

			////glClearColor(g.m_BackColor.x, g.m_BackColor.y, g.m_BackColor.z, 0.f);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			g.m_Shader.Bind();
			g.m_Shader.SetUniform1i("uIsWireframe", 0);
			g.m_Shader.SetUniform1i("uCam", 0);

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
					object.BindInstancedData<VertexBasic::MatID>(offset, count);
					object.BindInstancedData<VertexBasic::Material>(offset, count);
					// more binds...
					object.Draw(count);
				}

				object.Flush();
			}
		}
	);

	minimapPass.DrawThis(
		[&]()
		{
			// Effectively same as base pass

			if (renderOption == 1)
				return;

			auto view = EntityManager::View<Xform, Model>();
			for (auto [handle, xform, model] : view.each())
			{
				Rxuid meshID = model.GetMesh();
				if (meshID == RX_INVALID_ID)
					continue;

				Object<VertexBasic>& o = GetObjekt(meshID);
				o.Submit<VertexBasic::Xform>(xform.GetXform());
			}

			//glClearColor(g.m_BackColor.x, g.m_BackColor.y, g.m_BackColor.z, 0.f);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			g.m_Shader.Bind();
			g.m_Shader.SetUniform1i("uIsWireframe", 0);
			g.m_Shader.SetUniform1i("uCam", 1);

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
					// more binds...
					object.Draw(count);
				}

				object.Flush();
			}
		}
	);

	wireframePass.DrawThis(
		[&]()
		{
			if (renderOption == 0)
				return;

			//for (Entity& ent : EntityManager::GetEntities())
			//{
			//	auto& colDetails = ent.GetColliderDetails();
			//	if (colDetails.BVType == BV::NIL || !colDetails.pBV)
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
				BV bvType = collider.GetBVType();
				if (bvType == BV::NIL)
					continue;

				// Some triangle non-conforming hardcode.
				// Hopefully is temporary. I'd prefer to use the actual
				// triangle mesh.
				if (bvType == BV::Triangle)
				{
					TriangleBV& bv = EntityManager::GetComponent<TriangleBV>(handle);
					glm::vec3 from = RayBV::DefaultDirection;

					glm::vec3 p0 = bv.GetP0_W();
					glm::vec3 p1 = bv.GetP1_W();
					glm::vec3 p2 = bv.GetP2_W();

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
						o.Submit<VertexBasic::IsCollide>(bv.IsCollide());			\
					}
					_RX_X(p0, to0);
					_RX_X(p1, to1);
					_RX_X(p2, to2);
#undef _RX_X
					continue;
				}

#define _RX_X(Klass)																\
				if (bvType == BV::Klass)											\
				{																	\
					/*Should check ensure that get<BV> exists*/						\
					Klass##BV& bv = EntityManager::GetComponent<Klass##BV>(handle);	\
					Object<VertexBasic>& o = GetObjekt(bvType);						\
					o.Submit<VertexBasic::Xform>(bv.GetXform());					\
					o.Submit<VertexBasic::IsCollide>(bv.IsCollide());				\
				}
				RX_DO_ALL_BV_ENUM;
#undef _RX_X
			}

			// Extra per collider type stuff
			auto triangleView = EntityManager::View<TriangleBV>();
			for (auto [handle, bv] : triangleView.each())
			{
				{
					auto& obj = GetObjekt(BV::Point);
					//obj.Submit<VertexBasic::Xform>(glm::translate(bv.GetP0_W()));
					//obj.Submit<VertexBasic::Xform>(glm::translate(bv.GetP1_W()));
					//obj.Submit<VertexBasic::Xform>(glm::translate(bv.GetP2_W()));
					obj.Submit<VertexBasic::Xform>(glm::translate(bv.GetPosition()));
				}
				{
					auto& obj = GetObjekt(BV::Ray);
					glm::vec3 from = RayBV::DefaultDirection;
					glm::vec3 to = bv.GetNormal();

					glm::quat quat = glm::rotation(from, to);
					obj.Submit<VertexBasic::Xform>(glm::translate(bv.GetPosition()) * glm::mat4_cast(quat));
					obj.Submit<VertexBasic::IsCollide>(false);
				}
			}

			auto planeView = EntityManager::View<PlaneBV>();
			for (auto [handle, bv] : planeView.each())
			{
				{
					auto& obj = GetObjekt(BV::Point);
					obj.Submit<VertexBasic::Xform>(glm::translate(bv.GetPosition()));
				}
				{
					auto& obj = GetObjekt(BV::Ray);
					glm::vec3 from = RayBV::DefaultDirection;
					glm::vec3 to = bv.GetNormal();

					glm::quat quat = glm::rotation(from, to);
					obj.Submit<VertexBasic::Xform>(glm::translate(bv.GetPosition()) * glm::mat4_cast(quat));
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
			g.m_Shader.SetUniform3f("uWireframeColor", glm::vec3{ 0.f,1.f,0.f });
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
			glBindTexture(GL_TEXTURE_2D, basePass.m_TextureBuffer);
			g.m_FBOShader.SetUniform1i("uBaseTex", 0);
			g.m_FBOShader.SetUniform1i("uHasBaseTex", renderOption != 1);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, wireframePass.m_TextureBuffer);
			g.m_FBOShader.SetUniform1i("uWireframeTex", 1);
			g.m_FBOShader.SetUniform1i("uHasWireframeTex", renderOption != 0);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, minimapPass.m_TextureBuffer);
			g.m_FBOShader.SetUniform1i("uMinimapTex", 2);
			//g.m_FBOShader.SetUniform1i("uHasWireframeTex", renderOption != 0);

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

Object<VertexBasic>& RenderSystem::GetObjekt(BV bv)
{
	switch (bv)
	{
		case BV::Point:
			return GetObjekt(Shape::Point);
		case BV::Ray:
			return GetObjekt(Shape::Line);
		case BV::Triangle:
			return GetObjekt(Shape::Triangle);
		case BV::Plane:
			return GetObjekt(Shape::Plane);
		case BV::AABB:
			return GetObjekt(Shape::Cube);
		case BV::Sphere:
			return GetObjekt(Shape::Sphere);
		default:
			RX_ASSERT(false);
			return GetObjekt(Shape::Cube);
	}
}

void RenderSystem::CreateShapes()
{
	// Screen Quad
	{
		std::vector<GLuint> indices{
			0, 1, 2,
			2, 3, 0 
		};
		std::vector<glm::vec2> positions{
			{ -1.0f,  1.0f },
			{ -1.0f, -1.0f },
			{  1.0f, -1.0f },
			{  1.0f,  1.0f }
		};
		std::vector<glm::vec2> texCoords{
			{ 0.f, 1.f },
			{ 0.f, 0.f },
			{ 1.f, 0.f },
			{ 1.f, 1.f }
		};

		g.m_FBOObject.BeginObject(GL_TRIANGLES)
			.PushIndices(indices)
			.Push<VertexFBO::Position>(positions)
			.Push<VertexFBO::TexCoords>(texCoords)
			.EndObject();
	}

	CreateObjekt(CreatePoint());
	CreateObjekt(CreateLine());
	CreateObjekt(CreateTriangle());
	CreateObjekt(CreateQuad());
	CreateObjekt(CreatePlane());
	CreateObjekt(CreateCube());
	CreateObjekt(CreateSphere());
}

void RenderSystem::CreateObjekt(ObjectParams const& objParams)
{
	// Still gotta manually expand each .push if the vertex changes
	// No easy way to automatically inc the tuple's index through pre-proc. 
	// (not worth the timr rn)
#define _RX_TUP(i) std::get<i>(objParams)
#define _RX_X(Klass) .Push<Klass>(typename Klass::container_type{})
	auto& objekt = GetObjekt(_RX_TUP(0));
	objekt.BeginObject(_RX_TUP(1));

	auto& indices = _RX_TUP(2);
	if (indices.size()) 
		objekt.PushIndices(indices);

	objekt
		//.PushIndices(_RX_TUP(2))
		.Push<VertexBasic::Position>(_RX_TUP(3))
		.Push<VertexBasic::TexCoord>(_RX_TUP(4))
		.Push<VertexBasic::Normal>(_RX_TUP(5))
		RX_VERTEX_BASIC_ATTRIBS_M_INSTANCED(_RX_X)
		.EndObject();
#undef _RX_X

	// validate, all attribute containers should(?) be exactly the same size
	size_t sz = _RX_TUP(3).size();
	bool allSame = true;
#define _RX_X(Klass) allSame &= objekt.GetVBData<Klass>().size() == sz;
	RX_VERTEX_BASIC_ATTRIBS_M_NOINSTANCED(_RX_X);
#undef _RX_X
#undef _RX_TUP

	RX_ASSERT(allSame, "Might have forgotten to setup an attribute's data for some object.");
}


RenderSystem::ObjectParams RenderSystem::CreatePoint()
{ // Point
	std::vector<GLuint> indices{
		0, 1,
		2, 3,
		4, 5
	};
	VertexBasic::Position::container_type positions{
		{ -0.2f,  0.0f,  0.0f },  { 0.2f, 0.0f, 0.0f },
		{  0.0f, -0.2f,  0.0f },  { 0.0f, 0.2f, 0.0f },
		{  0.0f,  0.0f, -0.2f },  { 0.0f, 0.0f, 0.2f },
	};
	VertexBasic::TexCoord::container_type texCoords{};
	texCoords.resize(positions.size());
	VertexBasic::Normal::container_type normals{};
	normals.resize(positions.size());

	return ObjectParams{
		Shape::Point, GL_LINES,
		indices,
		positions,
		texCoords,
		normals,
		nullptr
	};
}

RenderSystem::ObjectParams RenderSystem::CreateLine()
{
	// inward facing is the agreed upon standard for ray
	std::vector<GLuint> indices{ 
		0, 1, 
	};
	VertexBasic::Position::container_type positions{
		{0.f,0.f,0.f}, {0.f,0.f,-1.f},
	};
	VertexBasic::TexCoord::container_type texCoords{};
	texCoords.resize(positions.size());
	VertexBasic::Normal::container_type normals{};
	normals.resize(positions.size(), RayBV::DefaultDirection);

	return ObjectParams{
		Shape::Line, GL_LINES,
		indices,
		positions,
		texCoords,
		normals,
		nullptr
	};
}

// An equilateral triangle, with 1 unit from center to vertex
RenderSystem::ObjectParams RenderSystem::CreateTriangle()
{
	VertexBasic::Position::container_type positions{
		TriangleBV::DefaultP0,
		TriangleBV::DefaultP1,
		TriangleBV::DefaultP2
	};
	VertexBasic::TexCoord::container_type texCoords{
		glm::vec2{ (positions[0] + 1.f) * 0.5f },
		glm::vec2{ (positions[1] + 1.f) * 0.5f },
		glm::vec2{ (positions[2] + 1.f) * 0.5f },
	};
	VertexBasic::Normal::container_type normals{ 3, TriangleBV::DefaultNormal };

	return ObjectParams{
		Shape::Triangle, GL_TRIANGLES,
		{},
		positions,
		texCoords,
		normals,
		nullptr
	};
}

RenderSystem::ObjectParams RenderSystem::CreateQuad()
{
	std::vector<GLuint> indices{
		0, 1, 2,
		2, 3, 0
	};
	std::vector<glm::vec3> positions{
		{ -0.5f,  0.5f,  0.f },
		{ -0.5f, -0.5f,  0.f },
		{  0.5f, -0.5f,  0.f },
		{  0.5f,  0.5f,  0.f },
	};
	VertexBasic::TexCoord::container_type texCoords{
		{ 0.f, 1.f },
		{ 0.f, 0.f },
		{ 1.f, 0.f },
		{ 1.f, 1.f },
	};
	VertexBasic::Normal::container_type normals{};
	normals.resize(positions.size(), PlaneBV::DefaultNormal);

	return ObjectParams{
		Shape::Quad, GL_TRIANGLES,
		indices,
		positions,
		texCoords,
		normals,
		nullptr
	};
}

RenderSystem::ObjectParams RenderSystem::CreatePlane()
{ // Will be a XY plane by default
	const uint32_t size = 20;
	const float edgeLength = 1.f;
	std::vector<GLuint> indices{};
	std::vector<glm::vec3> positions{};

	float b = (float)size * edgeLength * 0.5f;
	glm::vec3 startPos{ -b, -b, 0.f };
	glm::vec3 step{ edgeLength, edgeLength, 0.f };

	for (uint32_t row = 0; row <= size; ++row)
	{
		float y = startPos.y + (float)row * step.y;
		for (uint32_t col = 0; col <= size; ++col)
		{
			float x = startPos.x + (float)col * step.x;
			positions.emplace_back(std::move(glm::vec3{ x, y, 0.f }));
		}
	}

	for (GLuint row = 0; row < size; ++row)
	{
		GLuint start = row * (size + 1);
		for (GLuint col = 0; col < size; ++col)
		{
			GLuint a = start + col + size + 1;
			GLuint b = start + col;
			GLuint c = start + col + 1;
			GLuint d = start + col + size + 1 + 1;

			indices.push_back(a);
			indices.push_back(b);
			indices.push_back(c);

			indices.push_back(c);
			indices.push_back(d);
			indices.push_back(a);
		}
	}
	VertexBasic::TexCoord::container_type texCoords{};
	texCoords.resize(positions.size());
	VertexBasic::Normal::container_type normals{};
	normals.resize(positions.size(), PlaneBV::DefaultNormal);

	return ObjectParams{
		Shape::Plane, GL_TRIANGLES,
		indices,
		positions,
		texCoords,
		normals,
		nullptr
	};
}

RenderSystem::ObjectParams RenderSystem::CreateCube()
{
	std::vector<GLuint> indices{
		0, 1, 2, 2, 3, 0, // Front face
		4, 5, 6, 6, 7, 4, // Back face
		6, 5, 2, 2, 1, 6, // Bottom face
		0, 3, 4, 4, 7, 0, // Top face
		7, 6, 1, 1, 0, 7, // Left face
		3, 2, 5, 5, 4, 3  // Right face
	};
	VertexBasic::Position::container_type positions{
		{ -0.5f,  0.5f,  0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f }
	};
	VertexBasic::TexCoord::container_type texCoords{
		{ 0.f, 1.f },
		{ 0.f, 0.f },
		{ 1.f, 0.f },
		{ 1.f, 1.f },
		{ 0.f, 1.f },
		{ 0.f, 0.f },
		{ 1.f, 0.f },
		{ 1.f, 1.f }
	};

	VertexBasic::Normal::container_type normals{};
	// Generate normals.
	// Since we want normals, screw indexing and just duplicate all the vertices.
	// Kinda yucks but wtv man
	{
		VertexBasic::Position::container_type tempPositions{};
		VertexBasic::TexCoord::container_type tempTexCoords{};
		VertexBasic::Normal::container_type   tempNormals{};

		for (size_t i = 0; i < indices.size(); i += 3)
		{
			GLuint i0 = indices[i];
			GLuint i1 = indices[i + 1];
			GLuint i2 = indices[i + 2];

			glm::vec3 const& p0 = positions[i0];
			glm::vec3 const& p1 = positions[i1];
			glm::vec3 const& p2 = positions[i2];
			glm::vec3 norm = glm::cross(p1-p0, p2-p1);

			tempPositions.push_back(p0);
			tempPositions.push_back(p1);
			tempPositions.push_back(p2);

			tempTexCoords.push_back(texCoords[i0]);
			tempTexCoords.push_back(texCoords[i1]);
			tempTexCoords.push_back(texCoords[i2]);

			tempNormals.push_back(norm);
			tempNormals.push_back(norm);
			tempNormals.push_back(norm);
		}

		std::swap(positions, tempPositions);
		std::swap(texCoords, tempTexCoords);
		std::swap(normals, tempNormals);
		indices.clear(); // 
	}

	//texCoords.resize(positions.size());
	//normals.resize(positions.size());

	return ObjectParams{
		Shape::Cube, GL_TRIANGLES,
		indices,
		positions,
		texCoords,
		normals,
		nullptr
	};
}

// This sphere is bad becasue there are holes, and ALOT of duplicates vertices.
// Too lazy to fix it rn
RenderSystem::ObjectParams RenderSystem::CreateSphere(int refinement)
{ // https://blog.lslabs.dev/posts/generating_icosphere_with_code
	// Vertices stolen from https://schneide.blog/2016/07/15/generating-an-icosphere-in-c/
	std::vector<GLuint> indices{
		0,  1,  4,
		0,  4,  9,
		9,  4,  5,
		4,  8,  5,
		4,  1,  8,
		8,  1,  10,
		8,  10, 3,
		5,  8,  3,
		5,  3,  2,
		2,  3,  7,
		7,  3,  10,
		7,  10, 6,
		7,  6,  11,
		11, 6,  0,
		0,  6,  1,
		6,  10, 1,
		9,  11, 0,
		9,  2,  11,
		9,  5,  2,
		7,  11, 2 
	};

	const float X = 0.525731112119133606f;
	const float Z = 0.850650808352039932f;
	const float N = 0.f;
	std::vector<glm::vec3> positions{
		{-X, N,  Z}, { X, N,  Z}, {-X, N, -Z}, { X, N, -Z},
		{ N, Z,  X}, { N, Z, -X}, { N,-Z,  X}, { N,-Z, -X},
		{ Z, X,  N}, {-Z, X,  N}, { Z,-X,  N}, {-Z,-X,  N}
	};
	VertexBasic::TexCoord::container_type texCoords{};
	VertexBasic::Normal::container_type normals{};

	static auto MidPoint =
		[](glm::vec3 v0, glm::vec3 v1) -> glm::vec3
		{
			return glm::normalize(glm::normalize(v0) + glm::normalize(v1));
		};

	for (int r = 0; r < refinement; ++r)
	{
		decltype(positions) tempPositions{};
		decltype(normals) tempNormals{};
		decltype(indices) tempIndices{};

		for (int i = 0; (i+3) < indices.size(); i += 3)
		{
			GLuint index0 = indices[i];
			GLuint index1 = indices[i + 1];
			GLuint index2 = indices[i + 2];

			glm::vec3 const& v0 = positions[index0];
			glm::vec3 const& v1 = positions[index1];
			glm::vec3 const& v2 = positions[index2];
			glm::vec3 const& v3 = MidPoint(v0, v1);
			glm::vec3 const& v4 = MidPoint(v1, v2);
			glm::vec3 const& v5 = MidPoint(v2, v0);

			GLuint currIndex = tempPositions.size();
			tempPositions.push_back(v0);
			tempPositions.push_back(v1);
			tempPositions.push_back(v2);
			tempPositions.push_back(v3);
			tempPositions.push_back(v4);
			tempPositions.push_back(v5);

			tempNormals.push_back(glm::normalize(v0));
			tempNormals.push_back(glm::normalize(v1));
			tempNormals.push_back(glm::normalize(v2));
			tempNormals.push_back(glm::normalize(v3));
			tempNormals.push_back(glm::normalize(v4));
			tempNormals.push_back(glm::normalize(v5));

			tempIndices.push_back(currIndex);
			tempIndices.push_back(currIndex + 3);
			tempIndices.push_back(currIndex + 5);

			tempIndices.push_back(currIndex + 3);
			tempIndices.push_back(currIndex + 1);
			tempIndices.push_back(currIndex + 4);

			tempIndices.push_back(currIndex + 5);
			tempIndices.push_back(currIndex + 4);
			tempIndices.push_back(currIndex + 2);

			tempIndices.push_back(currIndex + 3);
			tempIndices.push_back(currIndex + 4);
			tempIndices.push_back(currIndex + 5);
		}

		positions = tempPositions;
		indices = tempIndices;
		normals = tempNormals;
	}
	texCoords.resize(positions.size());

	return ObjectParams{
		Shape::Sphere, GL_TRIANGLES,
		indices,
		positions,
		texCoords,
		normals,
		nullptr
	};
}
