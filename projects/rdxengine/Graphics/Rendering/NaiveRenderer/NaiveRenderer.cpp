#include "NaiveRenderer.h"
#include "ServiceLayer.h"

using namespace rdx;

namespace {
	constexpr int MAX_INSTANCES = 5;

	std::vector<GLuint> indices{
		0, 1, 2, 2, 3, 0, // Front face
		4, 5, 6, 6, 7, 4, // Back face
		6, 5, 2, 2, 1, 6, // Bottom face
		0, 3, 4, 4, 7, 0, // Top face
		7, 6, 1, 1, 0, 7, // Left face
		3, 2, 5, 5, 4, 3  // Right face
	};

}

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
			"#version 450 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"layout (location = 1) in mat4 aXform;\n"
			"uniform mat4 viewMat;\n"
			"uniform mat4 projMat;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = projMat * viewMat * aXform * vec4(aPos, 1.0);\n"
			"}\0";

		const char* fragmentShaderSource =
			"#version 450 core\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"   FragColor = vec4(1.f, 0.f, 1.f, 1.f);\n"
			"}\0";
		m_DefaultShader.Init({
			{Shader::Type::Vertex, vertexShaderSource},
			{Shader::Type::Fragment, fragmentShaderSource}
			});
	}
	RX_ASSERT(m_DefaultShader);

	{
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

		Mesh::VertexLayout layout{};
		layout.Push(Mesh::VertexAttribute{
			.AttributeType = Mesh::VertexAttributeType::Vec3,
			.AttributeCount = 1,
			.FundamentalType = Mesh::VertexAttributeType::Float,
			.FundamentalCount = 3,
			.Data = vertices.data(),
			.Length = vertices.size(),
			.IsInstanced = false,
			.IsNormalized = false
			});
		layout.Push(Mesh::VertexAttribute{
			.AttributeType = Mesh::VertexAttributeType::Vec4,
			.AttributeCount = 4,
			.FundamentalType = Mesh::VertexAttributeType::Float,
			.FundamentalCount = 4,
			.Data = nullptr,
			.Length = 0,
			.IsInstanced = true,
			.IsNormalized = false
			});
		m_DefaultMesh.Init(layout, indices);
	}
	RX_ASSERT(m_DefaultMesh);

	{
		bool passesOK = true;
		for (auto& pipeline : m_Pipelines)
		{
			for (auto pass : pipeline.Passes)
				passesOK &= pass.lock()->Init();
		}
		RX_ASSERT(passesOK);
	}

	return true;
}

bool NaiveRenderer::TerminateImpl()
{
	{
		bool passesOK = true;
		for (auto& pipeline : m_Pipelines)
		{
			for (auto pass : pipeline.Passes)
				passesOK &= pass.lock()->Terminate();
		}
		RX_ASSERT(passesOK);
	}

	m_DefaultShader.Terminate();
	return true;
}

void NaiveRenderer::DrawImpl()
{

	for (auto const& pipeline : m_Pipelines)
	{
		for (auto pass : pipeline.Passes)
		{
			pass.lock()->Draw();
		}
	}
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

#pragma region ::Mesh
bool NaiveRenderer::Mesh::Init(Mesh::VertexLayout const& layout, std::vector<GLuint> indices)
{
	glCreateVertexArrays(1, &m_VAO);

	if (!indices.empty())
	{
		GLuint ebo{};
		glCreateBuffers(1, &ebo);
		glNamedBufferData(ebo,
			indices.size() * sizeof(GLuint),
			indices.data(),
			GL_STATIC_DRAW);

		glVertexArrayElementBuffer(m_VAO, ebo);
	}

	GLuint location = 0;
	for (Mesh::VertexAttribute const& attrib : layout.Attributes)
	{
		size_t const fundamentalSize = GetFundamentalSize(attrib.FundamentalType);
		size_t const bufferSize = (attrib.IsInstanced ? MAX_INSTANCES : attrib.Length) * 
			attrib.AttributeCount * attrib.FundamentalCount * fundamentalSize;
		void* const bufferData = attrib.IsInstanced ? nullptr : attrib.Data;

		GLuint vbo{};
		glCreateBuffers(1, &vbo);
		glNamedBufferStorage(vbo, bufferSize, bufferData, GL_DYNAMIC_STORAGE_BIT);

		if (attrib.AttributeCount > 1)  // AOS: packed (e.g., mat4)
		{
			size_t stride = attrib.AttributeCount * attrib.FundamentalCount * fundamentalSize;
			GLuint bindingIndex = location;
			glVertexArrayVertexBuffer(m_VAO, bindingIndex, vbo, 0, stride);

			for (uint32_t i = 0; i < attrib.AttributeCount; ++i)
			{
				uint32_t currLocation = location + i;
				glEnableVertexArrayAttrib(m_VAO, currLocation);
				glVertexArrayAttribBinding(m_VAO, currLocation, bindingIndex);
				glVertexArrayAttribFormat(m_VAO, currLocation,
					attrib.FundamentalCount, TraslateAttribType(attrib.FundamentalType), // For this attrib, how many of GLenums?
					attrib.IsNormalized,
					i * attrib.FundamentalCount * fundamentalSize);  // Offset within stride

				if (attrib.IsInstanced)
					glVertexArrayBindingDivisor(m_VAO, currLocation, 1);
			}
		}
		else  // SOA: separate columns
		{
			size_t stride = attrib.FundamentalCount * fundamentalSize;

			uint32_t currLocation = location;
			GLuint bindingIndex = location;
			size_t offset = 0;// MAX_INSTANCES* attrib.FundamentalCount* fundamentalSize;

			glVertexArrayVertexBuffer(m_VAO, bindingIndex, vbo, offset, stride);
			glEnableVertexArrayAttrib(m_VAO, currLocation);
			glVertexArrayAttribBinding(m_VAO, currLocation, bindingIndex);
			glVertexArrayAttribFormat(m_VAO, currLocation,
				attrib.FundamentalCount, TraslateAttribType(attrib.FundamentalType), // For this attrib, how many of GLenums?
				attrib.IsNormalized,
				0);  // No offset since binding already points to the column

			if (attrib.IsInstanced)
				glVertexArrayBindingDivisor(m_VAO, currLocation, 1);
		}

		m_VBOs.push_back(vbo);
		location += attrib.AttributeCount;
	}

	return true;
}

bool NaiveRenderer::Mesh::Terminate()
{
	// Clean up ALL VAO, VBO, EBO

	return true;
}

void NaiveRenderer::Mesh::Bind() const
{
	glBindVertexArray(m_VAO);
}

size_t NaiveRenderer::Mesh::GetFundamentalSize(VertexAttributeType const t)
{
	switch (t)
	{
		case VertexAttributeType::Vec2:
		case VertexAttributeType::Vec3:
		case VertexAttributeType::Vec4:
		case VertexAttributeType::Float:
			return sizeof(float);
		case VertexAttributeType::UInt8:
			return sizeof(uint8_t);
		case VertexAttributeType::UInt32:
			return sizeof(uint32_t);
		default:
			RX_ASSERT(false);
			return 0;
	}
}

GLenum NaiveRenderer::Mesh::TraslateAttribType(VertexAttributeType const t)
{
	switch (t)
	{
	case VertexAttributeType::Vec2:
		return GL_FLOAT;
	case VertexAttributeType::Vec3:
		return GL_FLOAT;
	case VertexAttributeType::Vec4:
		return GL_FLOAT;
	case VertexAttributeType::UInt8:
		return GL_UNSIGNED_BYTE;
	case VertexAttributeType::UInt32:
		return GL_UNSIGNED_INT;
	case VertexAttributeType::Float:
		return GL_FLOAT;
	default:
		RX_ASSERT(false);
		return 0;
	}
}
#pragma endregion