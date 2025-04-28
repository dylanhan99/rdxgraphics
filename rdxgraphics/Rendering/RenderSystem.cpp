#include <pch.h>
#include "RenderSystem.h"

#include "GLFWWindow/GLFWWindow.h"
#include "Camera.h"

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

	//
	EventDispatcher<Camera&>::RegisterEvent(RX_EVENT_CAMERA_USER_TOGGLED,
		[](Camera& camera)
		{
			GLFWWindow::SetInvisibleCursor(camera.IsCameraInUserControl());
		});
	//

	//{
	//	glCreateFramebuffers(1, &m_FBO);
	//	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	//
	//	glGenTextures(1, &textureColorBuffer);
	//	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	//	glm::ivec2 dims = GLFWWindow::GetWindowDims();
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dims.x, dims.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//
	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);
	//}

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
	Object<VertexBasic>& object = GetObjekt(Shape::Sphere);

	//object.Submit<VertexBasic::Xform>(glm::translate(glm::vec3(move)));
	//object.Submit<VertexBasic::Xform>(glm::translate(glm::vec3(move * 2.f)));
	object.Submit<VertexBasic::Xform>(glm::translate(glm::vec3(move)));

	//auto& data = object.GetVBData<VertexBasic::Xform>();
	auto& data = object.GetVBData<VertexBasic::Xform>();

	basePass.DrawThis(
		[&]()
		{
			if (renderOption == 1)
				return;

			glClearColor(g.m_BackColor.x, g.m_BackColor.y, g.m_BackColor.z, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			g.m_Shader.Bind();
			g.m_Shader.SetUniformMatrix4f("uProjViewMatrix", mainCamera.GetProjMatrix() * mainCamera.GetViewMatrix());
			g.m_Shader.SetUniform1i("uIsWireframe", 0);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			object.Bind();

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			for (size_t count{ 0 }, offset{ 0 }; offset < data.size(); offset += count)
			{
				count = glm::min<size_t>(data.size() - offset, RX_MAX_INSTANCES);
				object.BindInstancedData<VertexBasic::Xform>(offset, count);
				object.Draw(count);
			}
		}
	);

	wireframePass.DrawThis(
		[&]()
		{
			if (renderOption == 0)
				return;

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

			object.Bind();

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			for (size_t count{ 0 }, offset{ 0 }; offset < data.size(); offset += count)
			{
				count = glm::min<size_t>(data.size() - offset, RX_MAX_INSTANCES);
				object.BindInstancedData<VertexBasic::Xform>(offset, count);
				object.Draw(count);
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

	data.clear();
	glBindVertexArray(0);
}

bool RenderSystem::ReloadShaders()
{
	return g.m_Shader.Reload();
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
			.Push<VertexBasic::Position>({{0.f,0.f,0.f}, {1.f,0.f,0.f} })
			.Push<VertexBasic::Xform>({})
			.EndObject();
	}

	{ // Quad
		std::vector<GLuint> indices{
			0, 1, 2,
			2, 3, 0 
		};
		std::vector<glm::vec3> positions{
			{ -0.5f,  0.5f,  0.5f },
			{ -0.5f, -0.5f,  0.5f },
			{  0.5f, -0.5f,  0.5f },
			{  0.5f,  0.5f,  0.5f }
		};

		GetObjekt(Shape::Quad).BeginObject(GL_TRIANGLES)
			.PushIndices(indices)
			.Push<VertexBasic::Position>(positions)
			.Push<VertexBasic::Xform>({})
			.EndObject();
	}

	{ // Plane
		const uint32_t size = 20;
		std::vector<GLuint> indices{ };
		std::vector<glm::vec3> positions{};

		float startPos = (float)size * 0.5f;
		float maxPos = startPos + size;
		for (float row = startPos; row < maxPos; row += 1.f)
		{
			for (float col = 0; col < maxPos; col += 1.f)
			{
				positions.emplace_back(col, 0.0f, row);
			}
		}

		// Generate indices for triangle strip
		for (GLuint y = 0; y < size; ++y)
		{
			if (y > 0) // Add a degenerate vertex (repeat first vertex) between rows
				indices.push_back((y + 0) * (size + 1));

			for (GLuint x = 0; x <= size; ++x)
			{
				indices.push_back((y + 0) * (size + 1) + x);
				indices.push_back((y + 1) * (size + 1) + x);
			}

			if (y < size - 1) // Add a degenerate vertex (repeat last vertex) between rows
				indices.push_back((y + 1) * (size + 1) + size);
		}

		GetObjekt(Shape::Plane).BeginObject(GL_TRIANGLE_STRIP)
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

		float a = 0.525731112119134f;
		float c = 0.85065080835157f;

		std::vector<GLuint> indices{
			8, 9, 10,
			8, 10, 3,
			8, 
		};
		std::vector<glm::vec3> positions{
			{ -c,  a, 0.f }, // XY plane
			{ -c, -a, 0.f },
			{  c, -a, 0.f },
			{  c,  a, 0.f },
			{ 0.f, -c,  a }, // YZ plane
			{ 0.f, -c, -a },
			{ 0.f,  c, -a },
			{ 0.f,  c,  a },
			{ -c, 0.f,  a }, // XZ plane
			{ -c, 0.f, -a },
			{  c, 0.f, -a },
			{  c, 0.f,  a }
		};

		float 

		GetObjekt(Shape::Sphere).BeginObject(GL_TRIANGLES)
			.PushIndices(indices)
			.Push<VertexBasic::Position>(positions)
			.Push<VertexBasic::Xform>({})
			.EndObject();
	}
}
