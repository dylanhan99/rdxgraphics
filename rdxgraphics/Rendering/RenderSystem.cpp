#include <pch.h>
#include "RenderSystem.h"

#include "GLFWWindow/GLFWWindow.h"
#include "Camera.h"
#include "Entity/EntityManager.h"

RX_SINGLETON_EXPLICIT(RenderSystem);
namespace fs = std::filesystem;

extern Camera mainCamera;

float move = 0.f;
int renderOption = 2;

//GLuint m_FBO{};
//GLuint textureColorBuffer{};
RenderPass basePass{};
RenderPass wireframePass{};
RenderPass finalPass{};


bool RenderSystem::Init()
{
	if (!gladLoadGL(glfwGetProcAddress)) 
	{
		RX_ERROR("Failed to initialize GLAD");
		return false;
	}

	RX_INFO("GL Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	g.m_Shader.Init({
		{ ShaderType::Vertex,	"Assets/default.vert" },
		{ ShaderType::Fragment, "Assets/default.frag" }
		});
	g.m_FBOShader.Init({
		{ ShaderType::Vertex,	"Assets/screen.vert" },
		{ ShaderType::Fragment, "Assets/screen.frag" }
		});

	CreateShapes();

	EventDispatcher<Camera&>::RegisterEvent(RX_EVENT_CAMERA_USER_TOGGLED,
		[](Camera& camera)
		{
			GLFWWindow::SetInvisibleCursor(camera.IsCameraInUserControl());
		});

	glm::ivec2 dims = GLFWWindow::GetWindowDims();
	basePass.Init(dims.x, dims.y);
	wireframePass.Init(dims.x, dims.y);
	finalPass.Init(nullptr);

	return true;
}

void RenderSystem::Terminate()
{
	for (auto& obj : g.m_Objects)
		obj.Terminate();

	g.m_Shader.Terminate();
}

void RenderSystem::Update(double dt)
{
	RX_UNREF_PARAM(dt);

	//Object<VertexBasic>& object = GetObjekt(Shape::Cube);
	//Object<VertexBasic>& object = GetObjekt(Shape::Plane);
	//
	////object.Submit<VertexBasic::Xform>(glm::translate(glm::vec3(move)));
	////object.Submit<VertexBasic::Xform>(glm::translate(glm::vec3(move * 2.f)));
	//object.Submit<VertexBasic::Xform>(glm::translate(glm::vec3(move)));
	//
	////auto& data = object.GetVBData<VertexBasic::Xform>();
	
	//auto& data = object.GetVBData<VertexBasic::Xform>();

	basePass.DrawThis(
		[&]()
		{
			if (renderOption == 1)
				return;

			for (Entity& ent : EntityManager::GetEntities())
			{
				auto& modelDetails = ent.GetModelDetails();

				Object<VertexBasic>& o = GetObjekt(ent.GetModelDetails().ShapeType);
				o.Submit<VertexBasic::Xform>(modelDetails.Xform);
			}

			glClearColor(g.m_BackColor.x, g.m_BackColor.y, g.m_BackColor.z, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			g.m_Shader.Bind();
			g.m_Shader.SetUniformMatrix4f("uProjViewMatrix", mainCamera.GetProjMatrix() * mainCamera.GetViewMatrix());
			g.m_Shader.SetUniform1i("uIsWireframe", 0);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			for (Object<VertexBasic>& object : g.m_Objects)
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

			for (Entity& ent : EntityManager::GetEntities())
			{
				auto& colDetails = ent.GetColliderDetails();
				if (colDetails.BVType == BV::NIL || !colDetails.pBV)
					continue;

				Object<VertexBasic>& o = GetObjekt(ent.GetColliderDetails().BVType);
				o.Submit<VertexBasic::Xform>(colDetails.pBV->GetXform());
			}

			//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT);
			//glDisable(GL_DEPTH_TEST); // (optional, just for debugging visibility)
			//glDisable(GL_BLEND);

			glClearColor(g.m_BackColor.x, g.m_BackColor.y, g.m_BackColor.z, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			g.m_Shader.Bind();
			g.m_Shader.SetUniformMatrix4f("uProjViewMatrix", mainCamera.GetProjMatrix() * mainCamera.GetViewMatrix());
			g.m_Shader.SetUniform1i("uIsWireframe", 1);
			g.m_Shader.SetUniform3f("uWireframeColor", glm::vec3{ 0.f,1.f,0.f });

			// First pass: Draw actual filled mesh
			glDisable(GL_CULL_FACE);
			//glCullFace(GL_BACK);

			//object.Bind();

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			for (Object<VertexBasic>& object : g.m_Objects)
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

	finalPass.DrawThis(
		[&]()
		{
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

	{ // Point
		GetObjekt(Shape::Point).BeginObject(GL_POINTS)
			.PushIndices({ 0 })
			.Push<VertexBasic::Position>({{0.f,0.f,0.f}})
			.Push<VertexBasic::Xform>({})
			.EndObject();
	}

	{ // Point
		GetObjekt(Shape::Line).BeginObject(GL_LINES)
			.PushIndices({ 0, 1 })
			.Push<VertexBasic::Position>({ {-0.5f,0.f,0.f}, {0.5f,0.f,0.f} })
			.Push<VertexBasic::Xform>({})
			.EndObject();
	}

	{ // Quad
		std::vector<GLuint> indices{
			0, 1, 2,
			2, 3, 0 
		};
		std::vector<glm::vec3> positions{
			{ -0.5f,  0.5f,  0.f },
			{ -0.5f, -0.5f,  0.f },
			{  0.5f, -0.5f,  0.f },
			{  0.5f,  0.5f,  0.f }
		};

		GetObjekt(Shape::Quad).BeginObject(GL_TRIANGLES)
			.PushIndices(indices)
			.Push<VertexBasic::Position>(positions)
			.Push<VertexBasic::Xform>({})
			.EndObject();
	}

	{ // Plane. Will be a XY plane by default
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

		GetObjekt(Shape::Plane).BeginObject(GL_TRIANGLES)
			.PushIndices(indices)
			.Push<VertexBasic::Position>(positions)
			.Push<VertexBasic::Xform>({})
			.EndObject();
	}

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

		GetObjekt(Shape::Cube).BeginObject(GL_TRIANGLES)
			.PushIndices(indices)
			.Push<VertexBasic::Position>(positions)
			.Push<VertexBasic::Xform>({})
			.EndObject();
	}

	{ // Sphere > https://blog.lslabs.dev/posts/generating_icosphere_with_code
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

		// Icosphering the icosahedron
		//int segments = 3;
		//{
		// https://github.com/egeozgul/3D-Icosphere-Generator-Library
		//	std::vector<GLuint> newIndices{};
		//	std::vector<glm::vec3> newPositions{};
		//}

		GetObjekt(Shape::Sphere).BeginObject(GL_TRIANGLES)
			.PushIndices(indices)
			.Push<VertexBasic::Position>(positions)
			.Push<VertexBasic::Xform>({})
			.EndObject();
	}
}
