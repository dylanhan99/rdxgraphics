#include <pch.h>
#include "RenderSystem.h"

#include "GLFWWindow/GLFWWindow.h"

RX_SINGLETON_EXPLICIT(RenderSystem);


unsigned int tVAO;
unsigned int tVBO;

unsigned int qVAO;
unsigned int qVBO;
unsigned int qEBO;

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

	glClearColor(g.m_BackColor.x, g.m_BackColor.y, g.m_BackColor.z, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

	glUseProgram(g.m_ShaderProgramID);
	if (false)
	{
		glBindVertexArray(tVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	else
	{
		glBindVertexArray(qVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}


	glBindVertexArray(0);
}

bool RenderSystem::ReloadShaders()
{
	const char* vertexShaderSource = 
		"#version 450 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
	const char* fragmentShaderSource = 
		"#version 450 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n\0";

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	g.m_ShaderProgramID = glCreateProgram();

	glAttachShader(g.m_ShaderProgramID, vertexShader);
	glAttachShader(g.m_ShaderProgramID, fragmentShader);
	glLinkProgram(g.m_ShaderProgramID);

	// Validate program health

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return true;
}

void RenderSystem::CreateShapes()
{
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
