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
RenderPass finalPass{};

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
	finalPass.Init(nullptr);

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
	RX_ASSERT(EntityManager::HasComponent<Camera>(camEnt), "Active camera entity is missing Camera component");
	Camera& activeCamera = EntityManager::GetComponent<Camera>(camEnt);
	Camera& minimapCamera = EntityManager::GetComponent<Camera>(miniEnt);

	basePass.DrawThis(
		[&]()
		{
			if (renderOption == 1)
				return;

			auto view = EntityManager::GetInstance().m_Registry.view<Xform, Model>();
			for (auto [handle, xform, model] : view.each())
			{
				Rxuid meshID = model.GetMesh();
				if (meshID == RX_INVALID_ID)
					continue;

				Object<VertexBasic>& o = GetObjekt(meshID);
				o.Submit<VertexBasic::Xform>(xform.GetXform());
			}

			glClearColor(g.m_BackColor.x, g.m_BackColor.y, g.m_BackColor.z, 0.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			g.m_Shader.Bind();
			g.m_Shader.SetUniformMatrix4f("uProjViewMatrix", activeCamera.GetProjMatrix() * activeCamera.GetViewMatrix());
			g.m_Shader.SetUniform1i("uIsWireframe", 0);

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

	minimapPass.DrawThis(
		[&]()
		{
			// Effectively same as base pass

			if (renderOption == 1)
				return;

			auto view = EntityManager::GetInstance().m_Registry.view<Xform, Model>();
			for (auto [handle, xform, model] : view.each())
			{
				Rxuid meshID = model.GetMesh();
				if (meshID == RX_INVALID_ID)
					continue;

				Object<VertexBasic>& o = GetObjekt(meshID);
				o.Submit<VertexBasic::Xform>(xform.GetXform());
			}

			glClearColor(g.m_BackColor.x, g.m_BackColor.y, g.m_BackColor.z, 0.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			g.m_Shader.Bind();
			g.m_Shader.SetUniformMatrix4f("uProjViewMatrix", minimapCamera.GetProjMatrix() * minimapCamera.GetViewMatrix());
			g.m_Shader.SetUniform1i("uIsWireframe", 0);

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
			auto view = EntityManager::GetInstance().m_Registry.view<Collider>();
			for (auto [handle, collider] : view.each())
			{
				BV bvType = collider.GetBVType();
				if (bvType == BV::NIL)
					continue;

				#define _RX_X(Klass)															   \
				if (bvType == BV::Klass)														   \
				{																				   \
					/*Should check ensure that get<BV> exists*/									   \
					Klass##BV& bv = EntityManager::GetInstance().m_Registry.get<Klass##BV>(handle);\
					Object<VertexBasic>& o = GetObjekt(bvType);									   \
					o.Submit<VertexBasic::Xform>(bv.GetXform());								   \
					o.Submit<VertexBasic::IsCollide>(bv.IsCollide());							   \
				}
				RX_DO_ALL_BV_ENUM;
				#undef _RX_X
			}

			//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT);
			//glDisable(GL_DEPTH_TEST); // (optional, just for debugging visibility)
			//glDisable(GL_BLEND);

			glClearColor(g.m_BackColor.x, g.m_BackColor.y, g.m_BackColor.z, 0.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			g.m_Shader.Bind();
			g.m_Shader.SetUniformMatrix4f("uProjViewMatrix", activeCamera.GetProjMatrix() * activeCamera.GetViewMatrix());
			g.m_Shader.SetUniform1i("uIsWireframe", 1);
			g.m_Shader.SetUniform3f("uWireframeColor", glm::vec3{ 0.f,1.f,0.f });

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

	finalPass.DrawThis(
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

			glActiveTexture(GL_TEXTURE3);
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
	objekt.BeginObject(_RX_TUP(1))
		.PushIndices(_RX_TUP(2))
		.Push<VertexBasic::Position>(_RX_TUP(3))
		.Push<VertexBasic::TexCoords>(_RX_TUP(4))
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
	VertexBasic::TexCoords::container_type texCoords{};
	texCoords.resize(positions.size());

	return ObjectParams{
		Shape::Point, GL_LINES,
		indices,
		positions,
		texCoords,
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
	VertexBasic::TexCoords::container_type texCoords{};
	texCoords.resize(positions.size());

	return ObjectParams{
		Shape::Line, GL_LINES,
		indices,
		positions,
		texCoords,
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
	VertexBasic::TexCoords::container_type texCoords{
		{ 0.f, 1.f },
		{ 0.f, 0.f },
		{ 1.f, 0.f },
		{ 1.f, 1.f },
	};

	return ObjectParams{
		Shape::Quad, GL_TRIANGLES,
		indices,
		positions,
		texCoords,
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
	VertexBasic::TexCoords::container_type texCoords{};
	texCoords.resize(positions.size());

	return ObjectParams{
		Shape::Plane, GL_TRIANGLES,
		indices,
		positions,
		texCoords,
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
	std::vector<glm::vec3> positions{
		{ -0.5f,  0.5f,  0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f }
	};
	VertexBasic::TexCoords::container_type texCoords{};
	texCoords.resize(positions.size());

	return ObjectParams{
		Shape::Cube, GL_TRIANGLES,
		indices,
		positions,
		texCoords,
		nullptr
	};
}

RenderSystem::ObjectParams RenderSystem::CreateSphere(int refinement)
{ // https://blog.lslabs.dev/posts/generating_icosphere_with_code
	// There were some explannations on how the icosphere is formed, but the basis of it
	// is working off the 3 planes the regular icosahedron is based on.
	// Tldr, center to a plane's point is 1 unit, while the ratio of width/height = goldenratio.
	// Using that understanding, we come to the following (half-height = a, half-width = c)
	// (assume width (c) is the longer edge of the plane)
	// v0-----v3	v0 - [-c,  a]
	// |     / |	v1 - [-c, -a]
	// |   m   |	v2 - [ c, -a]
	// | /     |	v3 - [ c,  a]
	// v1-----v2

	//float a = 0.525731112119134f;
	//float c = 0.85065080835157f;

	//std::vector<GLuint> indices{
	//	6, 0, 7,	// Upper 5
	//	6, 7, 3,
	//	6, 3, 10,
	//	6, 10, 9,
	//	6, 9, 0,
	//
	//	0, 8, 7,	// Middle 10
	//	7, 8, 11,
	//	7, 11, 3,
	//	3, 11, 2,
	//	3, 2, 10,
	//	10, 2, 5,
	//	10, 5, 9,
	//	9, 5, 1,
	//	9, 1, 0,
	//	0, 1, 8,
	//
	//	4, 2, 11,	// Lower 5
	//	4, 5, 2,
	//	4, 1, 5,
	//	4, 8, 1,
	//	4, 11, 8
	//};
	//std::vector<glm::vec3> positions{
	//	{ -c,  a, 0.f }, // XY plane
	//	{ -c, -a, 0.f },
	//	{  c, -a, 0.f },
	//	{  c,  a, 0.f },
	//	{ 0.f, -c,  a }, // YZ plane
	//	{ 0.f, -c, -a },
	//	{ 0.f,  c, -a },
	//	{ 0.f,  c,  a },
	//	{ -c, 0.f,  a }, // XZ plane
	//	{ -c, 0.f, -a },
	//	{  c, 0.f, -a },
	//	{  c, 0.f,  a }
	//};

	// Vertices stolen from https://github.com/lazysquirrellabs/sphere_generator/blob/361e4e64cc1b3ecd00db495181b4ec8adabcf37c/Assets/Libraries/SphereGenerator/Runtime/Generators/IcosphereGenerator.cs#L35
	std::vector<GLuint> indices{
		0,  1,  2,
		 0,  3,  1,
		 0,  2,  4,
		 3,  0,  5,
		 0,  4,  5,
		 1,  3,  6,
		 1,  7,  2,
		 7,  1,  6,
		 4,  2,  8,
		 7,  8,  2,
		 9,  3,  5,
		 6,  3,  9,
		 5,  4, 10,
		 4,  8, 10,
		 9,  5, 10,
		 7,  6, 11,
		 7, 11,  8,
		11,  6,  9,
		 8, 11, 10,
		10, 11,  9
	};
	std::vector<glm::vec3> positions{
		{0.8506508f,           0.5257311f,         0.f},
		{0.000000101405476f,   0.8506507f,        -0.525731f},
		{0.000000101405476f,   0.8506506f,         0.525731f},
		{0.5257309f,          -0.00000006267203f, -0.85065067f},
		{0.52573115f,         -0.00000006267203f,  0.85065067f},
		{0.8506508f,          -0.5257311f,         0.f},
		{-0.52573115f,         0.00000006267203f, -0.85065067f},
		{-0.8506508f,          0.5257311f,         0.f},
		{-0.5257309f,          0.00000006267203f,  0.85065067f},
		{-0.000000101405476f, -0.8506506f,        -0.525731f},
		{-0.000000101405476f, -0.8506507f,         0.525731f},
		{-0.8506508f,         -0.5257311f,         0.f}
	};
	VertexBasic::TexCoords::container_type texCoords{};
	texCoords.resize(positions.size());

	// Icosphering the icosahedron
	//int segments = 3;
	//{
	// https://github.com/egeozgul/3D-Icosphere-Generator-Library
	//	std::vector<GLuint> newIndices{};
	//	std::vector<glm::vec3> newPositions{};
	//}

	// slerp between the two points
	static auto MidPoint =
		[](glm::vec3 v0, glm::vec3 v1) -> glm::vec3
		{
			return 0.5f * (v0 + v1);
		};

	for (int r = 0; r < refinement; ++r)
	{
		decltype(positions) tempPositions{};
		decltype(indices) tempIndices{};

		for (int i = 0; (i+3) < indices.size(); i += 3)
		{
			GLuint index0 = indices[i];
			GLuint index1 = indices[i + 1];
			GLuint index2 = indices[i + 2];

			glm::vec3 const& v0 = positions[index0];
			glm::vec3 const& v1 = positions[index1];
			glm::vec3 const& v2 = positions[index2];

			GLuint currIndex = tempPositions.size();
			tempPositions.push_back(v0);
			tempPositions.push_back(v1);
			tempPositions.push_back(v2);
			tempPositions.push_back(MidPoint(v0, v1));
			tempPositions.push_back(MidPoint(v1, v2));
			tempPositions.push_back(MidPoint(v2, v0));

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
	}
	texCoords.resize(positions.size());

	return ObjectParams{
		Shape::Sphere, GL_TRIANGLES,
		indices,
		positions,
		texCoords,
		nullptr
	};
}
