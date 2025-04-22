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

	ReloadShaders();

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

	glDeleteProgram(g.m_ShaderProgramID);
}

void RenderSystem::Update(double dt)
{
	RX_UNREF_PARAM(dt);

	Object& cubeObject = GetObjekt(Shape::Cube);

	cubeObject.Submit(glm::translate(glm::vec3(move))); // Submit 1;
	cubeObject.Submit(glm::translate(glm::vec3(move * 2.f))); // Submit 1;

	glClearColor(g.m_BackColor.x, g.m_BackColor.y, g.m_BackColor.z, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto GetUniformLocation = [sid = g.m_ShaderProgramID](std::string const& name)->GLint
		{
			GLint loc = glGetUniformLocation(sid, name.c_str());
			if (loc == -1)
				RX_WARN("Failed to locate '%s' in shader.", name);
			return loc;
		};
	
	glUseProgram(g.m_ShaderProgramID);
	glUniformMatrix4fv(GetUniformLocation("uViewMatrix"), 1, GL_FALSE, &glm::value_ptr(mainCamera.GetViewMatrix())[0]);
	glUniformMatrix4fv(GetUniformLocation("uProjMatrix"), 1, GL_FALSE, &glm::value_ptr(mainCamera.GetProjMatrix())[0]);
	glUniform3fv(GetUniformLocation("uWireframeColor"), 1, glm::value_ptr(glm::vec3{ 0.f,1.f,0.f }));

	cubeObject.BindInstancedData();
	auto& data = cubeObject.m_Xforms;

	// Bind relevant VAO
	glBindVertexArray(cubeObject.m_VAO);

	if (renderOption == 0 || renderOption == 2)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glUniform1i(GetUniformLocation("uIsWireframe"), 0);

		// First pass: Draw actual filled mesh
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElementsInstanced(
				cubeObject.m_Primitive,
				cubeObject.m_Indices.size(),
				GL_UNSIGNED_INT,
				nullptr,
				data.size() // Actual count of live instances
			);
		}
	}

	if (renderOption == 1 || renderOption == 2)
	{
		glDisable(GL_CULL_FACE);

		glUniform1i(GetUniformLocation("uIsWireframe"), 1);
		// Second pass: Draw wireframe overlay
		{
			glEnable(GL_POLYGON_OFFSET_LINE);
			glPolygonOffset(-1.f, -1.f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glEnable(GL_LINE_SMOOTH);
			glLineWidth(1.f);
			glDrawElementsInstanced(
				cubeObject.m_Primitive,
				cubeObject.m_Indices.size(),
				GL_UNSIGNED_INT,
				nullptr,
				data.size() // Actual count of live instances
			);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Restore state
			glDisable(GL_POLYGON_OFFSET_LINE);
		}
	}

	data.clear();
	glBindVertexArray(0);
}

bool RenderSystem::ReloadShaders()
{
	if (g.m_ShaderProgramID)
	{
		glDeleteProgram(g.m_ShaderProgramID);
		g.m_ShaderProgramID = 0;
	}

	char infoLog[512]{};
	auto readAndCompile = 
		[&infoLog](GLint shaderType, fs::path const& shaderPath)
		{
			GLuint shaderID{ 0 };

			std::ifstream ifs{ shaderPath };
			if (!ifs)
			{
				RX_ERROR("Failed to read shader asset path: {}", shaderPath);
				return 0u;
			}

			std::string shaderBuffer{ std::istreambuf_iterator<char>{ifs}, {} };
			RX_DEBUG("{}\n{}", shaderPath, shaderBuffer);
			const char* shaderBufferCstr = shaderBuffer.c_str();

			shaderID = glCreateShader(shaderType);
			glShaderSource(shaderID, 1, &shaderBufferCstr, nullptr);
			glCompileShader(shaderID);

			int success{};
			glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
				RX_ERROR(R"(Failed to compile shader "{}" - {})", shaderPath, infoLog);
				return 0u;
			}

			return shaderID;
		};

	fs::path fpVert = g_WorkingDir; fpVert.append("Assets/default.vert");
	fs::path fpFrag = g_WorkingDir; fpFrag.append("Assets/default.frag");

	GLuint vertShader = readAndCompile(GL_VERTEX_SHADER, fpVert);
	GLuint fragShader = readAndCompile(GL_FRAGMENT_SHADER, fpFrag);

	int success = vertShader && fragShader;
	if (success)
	{
		g.m_ShaderProgramID = glCreateProgram();
		glAttachShader(g.m_ShaderProgramID, vertShader);
		glAttachShader(g.m_ShaderProgramID, fragShader);
		glLinkProgram(g.m_ShaderProgramID);

		glGetProgramiv(g.m_ShaderProgramID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(g.m_ShaderProgramID, 512, nullptr, infoLog);
			RX_ERROR("Failed to link shader program - {}", infoLog);
		}
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
	return success;
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
