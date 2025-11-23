#include "NaiveRenderer.h"
#include "ServiceLayer.h"

using namespace rdx;

extern GLuint tempVAO;
extern std::vector<GLuint> indices;
//std::vector<GLuint> indices{
//	0, 1, 2, 2, 3, 0, // Front face
//	4, 5, 6, 6, 7, 4, // Back face
//	6, 5, 2, 2, 1, 6, // Bottom face
//	0, 3, 4, 4, 7, 0, // Top face
//	7, 6, 1, 1, 0, 7, // Left face
//	3, 2, 5, 5, 4, 3  // Right face
//};

bool NaiveRenderer::InitImpl()
{
	{
		glewExperimental = GL_TRUE;

		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
			RX_CRITICAL("Failed to initialize GLEW: {}", (const char*)glewGetErrorString(glewError));
			return false;
		}

		// Clear the initial GLEW error that occurs with glewExperimental
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			//RX_WARN("Initial GLEW error (normal): {}", err);
		}
	}

	{
		glFrontFace(GL_CCW);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Set a default viewport (you might want to make this dynamic)
		glViewport(0, 0, 1280, 720); // Adjust to your window size
	}

	{
		const char* vertexShaderSource =
			"#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"uniform mat4 model;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = model * vec4(aPos, 1.0);\n"
			"}\0";

		const char* fragmentShaderSource =
			"#version 330 core\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
			"}\n";
		m_DefaultShader.Init({
			{Shader::Type::Vertex, vertexShaderSource},
			{Shader::Type::Fragment, fragmentShaderSource}
			});
	}
	RX_ASSERT(m_DefaultShader);

	{
		glGenVertexArrays(1, &tempVAO);
		glBindVertexArray(tempVAO);

		// Vertex data
		std::vector<glm::vec3> vertices{
			{ -0.5f,  0.5f,  0.5f }, // 0
			{ -0.5f, -0.5f,  0.5f }, // 1
			{  0.5f, -0.5f,  0.5f }, // 2
			{  0.5f,  0.5f,  0.5f }, // 3
			{  0.5f,  0.5f, -0.5f }, // 4
			{  0.5f, -0.5f, -0.5f }, // 5
			{ -0.5f, -0.5f, -0.5f }, // 6
			{ -0.5f,  0.5f, -0.5f }  // 7
		};

		// Setup Element Buffer Object (EBO) for indices
		GLuint ebo{};
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			indices.size() * sizeof(GLuint),
			indices.data(),
			GL_STATIC_DRAW);

		// Setup Vertex Buffer Object (VBO) for vertex positions
		GLuint vbo{};
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER,
			vertices.size() * sizeof(glm::vec3),
			vertices.data(),
			GL_STATIC_DRAW);

		// Setup vertex attribute pointer for position (location = 0)
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,                    // location = 0
			3,                    // 3 components (x, y, z)
			GL_FLOAT,             // type
			GL_FALSE,             // normalized
			sizeof(glm::vec3),    // stride (12 bytes)
			(void*)0);            // offset (0 bytes)

		// Unbind VAO first (important!), then other buffers
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	return true;
}

bool NaiveRenderer::TerminateImpl()
{
	m_DefaultShader.Terminate();
	return true;
}

void NaiveRenderer::DrawImpl()
{
	glm::vec4 m_BackbufferColor{ 0.2f, 0.3f, 0.3f, 1.0f };
	glClearColor(m_BackbufferColor.r, m_BackbufferColor.g, m_BackbufferColor.b, m_BackbufferColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(tempVAO);
	m_DefaultShader.Bind();
	RX_ECS_VIEWEACH(TransformComponent)(
		[&](EntityID eid, TransformComponent& xform)
		{
			// glm::mat4 const& modelXform = xform.GetTransformMatrix();
			glm::mat4 modelXform = glm::translate(glm::mat4{ 1.f }, xform.Position);
			m_DefaultShader.SetUniformMatrix4f("model", modelXform);

			// FIXED: Draw all indices with correct parameters
			glDrawElements(
				GL_TRIANGLES,
				static_cast<GLsizei>(indices.size()),  // Number of indices to draw
				GL_UNSIGNED_INT,
				nullptr  // We're using bound EBO, so no pointer needed
			);
		});
	m_DefaultShader.Unbind();
}

#pragma region ::Shader
bool NaiveRenderer::Shader::Init(std::initializer_list<std::pair<Type, std::string>> shaderList)
{
	std::vector<GLuint> shaderIDs{}; shaderIDs.reserve((size_t)Type::MAX);
	for (auto const& [t, buffer] : shaderList)
	{
		GLenum shaderType = TranslateShaderType(t);
		GLuint shaderID = LoadShader(shaderType, buffer);
		if (shaderID <= 0)
			RX_WARN("Failed to load the following {} shader:\n{}", SHADERTYPE_STRINGS[(size_t)t]);
		else
			shaderIDs.emplace_back(shaderID);
	}

	bool success = LinkShaderProgram(shaderIDs);
	if (!success)
		Terminate();

	CleanupShaders(shaderIDs);
	return success;
}

bool NaiveRenderer::Shader::Terminate()
{
	glDeleteProgram(m_ProgramID);
	m_ProgramID = 0;
	return true;
}

void NaiveRenderer::Shader::Bind() const
{
	glUseProgram(m_ProgramID);
}

void NaiveRenderer::Shader::Unbind() const
{
	glUseProgram(0);
}

void NaiveRenderer::Shader::SetUniform1i(std::string_view const& name, bool v)
{
	glUniform1i(GetUniformLocation(name), (int)v);
}

void NaiveRenderer::Shader::SetUniform1i(std::string_view const& name, int const& v)
{
	glUniform1i(GetUniformLocation(name), v);
}

void NaiveRenderer::Shader::SetUniform3f(std::string_view const& name, glm::vec3 const& v)
{
	glUniform3fv(GetUniformLocation(name), 1, &glm::value_ptr(v)[0]);
}

void NaiveRenderer::Shader::SetUniform3fv(std::string_view const& name, std::vector<glm::vec3> const& v)
{
	glUniform3fv(GetUniformLocation(name), (GLsizei)v.size(), glm::value_ptr(v[0]));
}

void NaiveRenderer::Shader::SetUniform4f(std::string_view const& name, glm::vec4 const& v)
{
	glUniform4fv(GetUniformLocation(name), 1, &glm::value_ptr(v)[0]);
}

void NaiveRenderer::Shader::SetUniformMatrix4f(std::string_view const& name, glm::mat4 const& v)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &glm::value_ptr(v)[0]);
}

void NaiveRenderer::Shader::SetUniformMatrix4fv(std::string_view const& name, std::vector<glm::mat4> const& v)
{
	glUniformMatrix4fv(GetUniformLocation(name), (GLsizei)v.size(), GL_FALSE, &glm::value_ptr(v[0])[0]);
}

GLint NaiveRenderer::Shader::GetUniformLocation(std::string_view const& name) const
{
	GLint location = glGetUniformLocation(m_ProgramID, name.data());
	if (location < 0)
		RX_WARN("Failed to locate uniform {} in Shader {}", name.data(), m_ProgramID);
	return location;
}

GLenum NaiveRenderer::Shader::TranslateShaderType(Type const t)
{
	switch (t)
	{
	case Type::Vertex:
		return GL_VERTEX_SHADER;
	case Type::Fragment:
		return GL_FRAGMENT_SHADER;
	case Type::Geometry:
		return GL_GEOMETRY_SHADER;
	case Type::Compute:
		return GL_COMPUTE_SHADER;
	default:
		RX_ASSERT(false);
		return 0;
	}
}

GLuint NaiveRenderer::Shader::LoadShader(GLenum shaderType, std::string const& buffer)
{
	GLuint id = glCreateShader(shaderType);

	const char* buffercstr = buffer.c_str();
	glShaderSource(id, 1, &buffercstr, nullptr);
	glCompileShader(id);

	int success{};
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512]{};
		glGetShaderInfoLog(id, 512, nullptr, infoLog);
		RX_ERROR("Failed to compile shader - {}\n{}", infoLog, buffer);

		glDeleteShader(id);
		return 0u;
	}

	return id;
}

bool NaiveRenderer::Shader::LinkShaderProgram(std::vector<GLuint> const& shaderIDs)
{
	RX_ASSERT([&shaderIDs]() { for (GLuint const& i : shaderIDs) { if (i <= 0) return false; } return true; }());

	m_ProgramID = glCreateProgram();
	for (GLuint id : shaderIDs)
		glAttachShader(m_ProgramID, id);

	GLint success = 1;
	glLinkProgram(m_ProgramID);
	glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512]{};
		glGetProgramInfoLog(m_ProgramID, 512, nullptr, infoLog);
		RX_ERROR("Failed to link shader program - {}", infoLog);
	}

	return success;
}

void NaiveRenderer::Shader::CleanupShaders(std::vector<GLuint> const& shaderIDs)
{
	for (GLuint const& id : shaderIDs)
		glDeleteShader(id);
}
#pragma endregion