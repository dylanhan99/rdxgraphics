#include <pch.h>
#include "RenderSystem.h"

#include "GLFWWindow/GLFWWindow.h"
#include "Camera.h"

RX_SINGLETON_EXPLICIT(RenderSystem);

unsigned int tVAO;
unsigned int tVBO;

unsigned int qVAO;
unsigned int qVBO;
unsigned int qEBO;

namespace fs = std::filesystem;

extern Camera mainCamera;

Object cubeObject{};
float move = 0.f;
bool triangles = true;

bool RenderSystem::Init()
{
	if (!gladLoadGL(glfwGetProcAddress)) 
	{
		RX_ERROR("Failed to initialize GLAD");
		return false;
	}

	RX_INFO("GL Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

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
	glDeleteVertexArrays(1, &tVAO);
	glDeleteVertexArrays(1, &qVAO);

	glDeleteBuffers(1, &tVBO);
	glDeleteBuffers(1, &qVBO);

	glDeleteBuffers(1, &qEBO);

	glDeleteProgram(g.m_ShaderProgramID);
}

void RenderSystem::Update(double dt)
{
	RX_UNREF_PARAM(dt);

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

	//if (false)
	//{
	//	glBindVertexArray(tVAO);
	//	glDrawArrays(GL_TRIANGLES, 0, 3);
	//}
	//else
	//{
	//	glBindVertexArray(qVAO);
	//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//}

	glBindBuffer(GL_ARRAY_BUFFER, cubeObject.m_VBOs[(size_t)Object::VertexAttrib::Xform_Inst]);
	auto& data = cubeObject.m_Xforms;
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::mat4), data.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(cubeObject.m_VAO);
	glDrawElementsInstanced(
		triangles ? GL_TRIANGLES : GL_LINE_LOOP,
		cubeObject.m_Indices.size(),
		GL_UNSIGNED_INT,
		nullptr,
		data.size() // Actual count of live instances
	);

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
	{
		cubeObject.m_Indices = std::vector<GLuint>{
			0, 1, 2, 2, 3, 0, // Front face
			4, 5, 6, 6, 7, 4, // Back face
			0, 1, 5, 5, 4, 0, // Bottom face
			2, 3, 7, 7, 6, 2, // Top face
			0, 3, 7, 7, 4, 0, // Left face
			1, 2, 6, 6, 5, 1  // Right face
		};

		cubeObject.m_Positions = std::vector<glm::vec3>{
			{ -0.5f, -0.5f, -0.5f },
			{  0.5f, -0.5f, -0.5f },
			{  0.5f,  0.5f, -0.5f },
			{ -0.5f,  0.5f, -0.5f },
			{ -0.5f, -0.5f,  0.5f },
			{  0.5f, -0.5f,  0.5f },
			{  0.5f,  0.5f,  0.5f },
			{ -0.5f,  0.5f,  0.5f }
		};

		glGenVertexArrays(1, &cubeObject.m_VAO);
		glBindVertexArray(cubeObject.m_VAO);

		// Position buffer
		{
			size_t attribI = (size_t)Object::VertexAttrib::Position;
			GLuint& attrib = cubeObject.m_VBOs[attribI];
			glGenBuffers(1, &attrib);
			glBindBuffer(GL_ARRAY_BUFFER, attrib);
			auto& data = cubeObject.m_Positions;
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec3), data.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray((GLuint)attribI);
			glVertexAttribPointer((GLuint)attribI, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // 3 x GL_FLOAT (glm::vec3)
		}

		// Xform buffer (Instanced)
		{
			size_t attribI = (size_t)Object::VertexAttrib::Xform_Inst;
			GLuint& attrib = cubeObject.m_VBOs[attribI];
			glGenBuffers(1, &attrib);
			glBindBuffer(GL_ARRAY_BUFFER, attrib);
			auto& data = cubeObject.m_Xforms;
			glBufferData(GL_ARRAY_BUFFER, /*maxExpectedInstances*/1000 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
			for (GLuint i = 0; i < 4; ++i) {
				GLuint id = (GLuint)attribI + i;
				glEnableVertexAttribArray(id);
				glVertexAttribPointer(id, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
				glVertexAttribDivisor(id, 1); // Advance per instance
			}
		}

		// Index buffer
		auto& indices = cubeObject.m_Indices;
		glGenBuffers(1, &cubeObject.m_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeObject.m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

		// Done
		glBindVertexArray(0);
	}

	{
		float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
		};

		{
			glGenVertexArrays(1, &tVAO);
		}
		{
			glBindVertexArray(tVAO);

			glGenBuffers(1, &tVBO);
			glBindBuffer(GL_ARRAY_BUFFER, tVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
		}
	
		glBindVertexArray(0);
	}

	{
		float vertices[] = {
			 0.5f,  0.5f, 0.0f,  // top right
			 0.5f, -0.5f, 0.0f,  // bottom right
			-0.5f, -0.5f, 0.0f,  // bottom left
			-0.5f,  0.5f, 0.0f   // top left 
		};
		unsigned int indices[] = {  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};

		{
			glGenVertexArrays(1, &qVAO);
			glGenBuffers(1, &qVBO);
			glGenBuffers(1, &qEBO);

			glBindVertexArray(qVAO);

			glBindBuffer(GL_ARRAY_BUFFER, qVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, qEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
		}
	
		glBindVertexArray(0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
