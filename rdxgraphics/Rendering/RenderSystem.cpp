#include <pch.h>
#include "RenderSystem.h"

#include "GLFWWindow/GLFWWindow.h"
#include "Camera.h"

RX_SINGLETON_EXPLICIT(RenderSystem);
namespace fs = std::filesystem;

extern Camera mainCamera;

float move = 0.f;
int renderOption = 2;

GLuint m_FBO{};
GLuint textureColorBuffer{};

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

	{
		glCreateFramebuffers(1, &m_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

		glGenTextures(1, &textureColorBuffer);
		glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);
	}

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

	Object<VertexBasic>& cubeObject = GetObjekt(Shape::Cube);

	cubeObject.Submit<VertexBasic::Xform>(glm::translate(glm::vec3(move)));
	cubeObject.Submit<VertexBasic::Xform>(glm::translate(glm::vec3(move * 2.f)));

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glClearColor(g.m_BackColor.x, g.m_BackColor.y, g.m_BackColor.z, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	g.m_Shader.Bind();
	g.m_Shader.SetUniformMatrix4f("uProjViewMatrix", mainCamera.GetProjMatrix() * mainCamera.GetViewMatrix());
	g.m_Shader.SetUniform3f("uWireframeColor", glm::vec3{ 0.f,1.f,0.f });

	auto& data = cubeObject.GetVBData<VertexBasic::Xform>();

	// First pass: Draw actual filled mesh
	if (renderOption == 0 || renderOption == 2)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		g.m_Shader.SetUniform1i("uIsWireframe", 0);
		cubeObject.Bind();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		for (size_t count{ 0 }, offset{ 0 }; offset < data.size(); offset += count)
		{
			count = glm::min<size_t>(data.size() - offset, RX_MAX_INSTANCES);
			cubeObject.BindInstancedData<VertexBasic::Xform>(offset, count);
			cubeObject.Draw(count);
		}
		//for (size_t i = 0; i < data.size(); i += RX_MAX_INSTANCES)
		//{
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//	cubeObject.Draw();
		//}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST); // (optional, just for debugging visibility)
	glDisable(GL_BLEND);

	g.m_FBOShader.Bind();
	//g.m_FBOShader.SetUniform3f("uWireframeColor", glm::vec3{ 0.f,1.f,0.f });
	
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);

	g.m_FBOObject.Bind();
	g.m_FBOObject.Draw(1);

	//if (renderOption == 1 || renderOption == 2)
	//{
	//	glDisable(GL_CULL_FACE);
	//
	//	g.m_Shader.SetUniform1i("uIsWireframe", 0);
	//	// Second pass: Draw wireframe overlay
	//	{
	//		glEnable(GL_POLYGON_OFFSET_LINE);
	//		glPolygonOffset(-1.f, -1.f);
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//
	//		glEnable(GL_LINE_SMOOTH);
	//		glLineWidth(1.f);
	//		glDrawElementsInstanced(
	//			cubeObject.m_Primitive,
	//			cubeObject.m_Indices.size(),
	//			GL_UNSIGNED_INT,
	//			nullptr,
	//			data.size() // Actual count of live instances
	//		);
	//
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Restore state
	//		glDisable(GL_POLYGON_OFFSET_LINE);
	//	}
	//}

	data.clear();
	glBindVertexArray(0);
}

bool RenderSystem::ReloadShaders()
{
	return g.m_Shader.Reload();
}

void RenderSystem::CreateShapes()
{
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

		Object<VertexBasic>& cubeObject = GetObjekt(Shape::Cube);
		cubeObject.BeginObject(GL_TRIANGLES)
			.PushIndices(indices)
			.Push<VertexBasic::Position>(positions)
			.Push<VertexBasic::Xform>(typename VertexBasic::Xform::container_type{})
			.EndObject();
	}

	{
		std::vector<GLuint> indices {  // note that we start from 0!
			0, 1, 2,   // first triangle
			2, 3, 0    // second triangle
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

	//{
	//	float vertices[] = {
	//	-0.5f, -0.5f, 0.0f,
	//	 0.5f, -0.5f, 0.0f,
	//	 0.0f,  0.5f, 0.0f
	//	};
	//
	//	{
	//		glGenVertexArrays(1, &tVAO);
	//	}
	//	{
	//		glBindVertexArray(tVAO);
	//
	//		glGenBuffers(1, &tVBO);
	//		glBindBuffer(GL_ARRAY_BUFFER, tVBO);
	//		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//
	//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//		glEnableVertexAttribArray(0);
	//	}
	//
	//	glBindVertexArray(0);
	//}
	//
	//{
	//	float vertices[] = {
	//		 0.5f,  0.5f, 0.0f,  // top right
	//		 0.5f, -0.5f, 0.0f,  // bottom right
	//		-0.5f, -0.5f, 0.0f,  // bottom left
	//		-0.5f,  0.5f, 0.0f   // top left 
	//	};
	//	unsigned int indices[] = {  // note that we start from 0!
	//		0, 1, 3,   // first triangle
	//		1, 2, 3    // second triangle
	//	};
	//
	//	{
	//		glGenVertexArrays(1, &qVAO);
	//		glGenBuffers(1, &qVBO);
	//		glGenBuffers(1, &qEBO);
	//
	//		glBindVertexArray(qVAO);
	//
	//		glBindBuffer(GL_ARRAY_BUFFER, qVBO);
	//		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//
	//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, qEBO);
	//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//
	//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//		glEnableVertexAttribArray(0);
	//	}
	//
	//	glBindVertexArray(0);
	//}

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
