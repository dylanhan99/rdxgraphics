#include <pch.h>
#include "RenderSystem.h"

#include "GLFWWindow/GLFWWindow.h"
#include "Camera.h"

RX_SINGLETON_EXPLICIT(RenderSystem);
namespace fs = std::filesystem;

extern Camera mainCamera;

float move = 0.f;
int renderOption = 2;

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

	CreateShapes();

	//
	EventDispatcher<Camera&>::RegisterEvent(RX_EVENT_CAMERA_USER_TOGGLED,
		[](Camera& camera)
		{
			GLFWWindow::SetInvisibleCursor(camera.IsCameraInUserControl());
		});
	//

	return true;
}

void RenderSystem::Terminate()
{
	for (Object& obj : g.m_Objects)
		obj.Terminate();

	g.m_Shader.Terminate();
}

void RenderSystem::Update(double dt)
{
	RX_UNREF_PARAM(dt);

	Object& cubeObject = GetObjekt(Shape::Cube);

	cubeObject.Submit(glm::translate(glm::vec3(move))); // Submit 1;
	cubeObject.Submit(glm::translate(glm::vec3(move * 2.f))); // Submit 1;

	glClearColor(g.m_BackColor.x, g.m_BackColor.y, g.m_BackColor.z, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	g.m_Shader.Bind();
	g.m_Shader.SetUniformMatrix4f("uViewMatrix", mainCamera.GetViewMatrix());
	g.m_Shader.SetUniformMatrix4f("uProjMatrix", mainCamera.GetProjMatrix());
	g.m_Shader.SetUniform3f("uWireframeColor", glm::vec3{ 0.f,1.f,0.f });

	auto& data = cubeObject.GetVBData<Vertex::Xform>();

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
			cubeObject.BindInstancedData<Vertex::Xform>(offset, count);
			cubeObject.Draw(count);
		}
		//for (size_t i = 0; i < data.size(); i += RX_MAX_INSTANCES)
		//{
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//	cubeObject.Draw();
		//}
	}

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
	Object::MakeObject(
		GetObjekt(Shape::Cube), GL_TRIANGLES,
		std::vector<GLuint>{
			0, 1, 2, 2, 3, 0, // Front face
			4, 5, 6, 6, 7, 4, // Back face
			6, 5, 2, 2, 1, 6, // Bottom face
			0, 3, 4, 4, 7, 0, // Top face
			7, 6, 1, 1, 0, 7, // Left face
			3, 2, 5, 5, 4, 3  // Right face
		},
		std::vector<glm::vec3>{
			{ -0.5f,  0.5f,  0.5f },
			{ -0.5f, -0.5f,  0.5f },
			{  0.5f, -0.5f,  0.5f },
			{  0.5f,  0.5f,  0.5f },
			{  0.5f,  0.5f, -0.5f },
			{  0.5f, -0.5f, -0.5f },
			{ -0.5f, -0.5f, -0.5f },
			{ -0.5f,  0.5f, -0.5f }
		});

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
