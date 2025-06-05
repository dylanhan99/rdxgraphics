#include "Shader.h"

namespace fs = std::filesystem;

bool Shader::Init(std::vector<std::pair<ShaderType, std::string>> const& shaderAssets)
{
	std::vector<GLuint> shaderIDs{};

	// Loading
	for (auto const& [t, b] : shaderAssets)
	{
		m_ShaderAssets[(size_t)t] = fs::path{};

		GLenum shaderType = GetShaderGLenum(t);
		GLuint shaderID = LoadShader(shaderType, b);
		shaderIDs.emplace_back(shaderID);
	}

	return LinkAndValidateShaderProgram(shaderIDs, m_ShaderProgramID);
}

bool Shader::Init(std::vector<std::pair<ShaderType, std::filesystem::path>> const& shaderAssets)
{
	std::vector<GLuint> shaderIDs{};

	// Loading
	for (auto const& [t, p] : shaderAssets)
	{
		m_ShaderAssets[(size_t)t] = p;

		GLenum shaderType = GetShaderGLenum(t);
		GLuint shaderID = LoadShader(shaderType, p);
		shaderIDs.emplace_back(shaderID);
	}

	return LinkAndValidateShaderProgram(shaderIDs, m_ShaderProgramID);
}

void Shader::Terminate()
{
	glDeleteProgram(m_ShaderProgramID);
	m_ShaderProgramID = 0;
}

bool Shader::Reload()
{
#if USE_CSD3151_AUTOMATION == 1
	// Prevent this usage in baked-in mode
	return true;
#else
	Terminate();
	std::vector<std::pair<ShaderType, fs::path>> v{};
	for (size_t i = 0; i < m_ShaderAssets.size(); ++i)
	{
		fs::path const& p = m_ShaderAssets[i];
		if (!p.empty())
			v.emplace_back(std::make_pair<ShaderType, fs::path>((ShaderType)i, fs::path(p)));
	}

	return Init(v);
#endif
}

void Shader::Bind() const
{
	glUseProgram(m_ShaderProgramID);
}

void Shader::SetUniform1i(std::string const& name, bool v)
{
	glUniform1i(GetUniformLocation(name), (int)v);
}

void Shader::SetUniform1i(std::string const& name, int const& v)
{
	glUniform1i(GetUniformLocation(name), v);
}

void Shader::SetUniform3f(std::string const& name, glm::vec3 const& v)
{
	glUniform3fv(GetUniformLocation(name), 1, &glm::value_ptr(v)[0]);
}

void Shader::SetUniform3fv(std::string const& name, std::vector<glm::vec3> const& v)
{
	glUniform3fv(GetUniformLocation(name), (GLsizei)v.size(), glm::value_ptr(v[0]));
}

void Shader::SetUniform4f(std::string const& name, glm::vec4 const& v)
{
	glUniform4fv(GetUniformLocation(name), 1, &glm::value_ptr(v)[0]);
}

void Shader::SetUniformMatrix4f(std::string const& name, glm::mat4 const& v)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &glm::value_ptr(v)[0]);
}

void Shader::SetUniformMatrix4fv(std::string const& name, std::vector<glm::mat4> const& v)
{
	glUniformMatrix4fv(GetUniformLocation(name), (GLsizei)v.size(), GL_FALSE, &glm::value_ptr(v[0])[0]);
}

GLuint Shader::LoadShader(GLenum shaderType, std::filesystem::path const& shaderPath)
{
	std::ifstream ifs{ shaderPath };
	if (!ifs)
	{
		RX_ERROR("Failed to read shader asset path: {}", shaderPath);
		return 0u;
	}

	RX_DEBUG("{}", shaderPath);
	std::string shaderBuffer{ std::istreambuf_iterator<char>{ifs}, {} };
	return LoadShader(shaderType, shaderBuffer);
}

GLuint Shader::LoadShader(GLenum shaderType, std::string const& shaderBuffer)
{
	GLuint shaderID{ 0 };
	RX_DEBUG("{}", shaderBuffer);
	const char* shaderBufferCstr = shaderBuffer.c_str();

	shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, &shaderBufferCstr, nullptr);
	glCompileShader(shaderID);

	int success{};
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512]{};
		glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
		RX_ERROR(R"(Failed to compile shader - {})", infoLog);
		return 0u;
	}

	return shaderID;
}

bool Shader::LinkAndValidateShaderProgram(std::vector<GLuint>& shaderIDs, GLuint& shaderProgramID)
{
	GLint success = 1;
	for (GLuint shaderID : shaderIDs)
		success = (bool)success & (bool)shaderID;

	if (!success)
	{
		for (GLuint shaderID : shaderIDs)
			glDeleteShader(shaderID);
		return false;
	}

	// Linking
	shaderProgramID = glCreateProgram();
	for (GLuint shaderID : shaderIDs)
		glAttachShader(shaderProgramID, shaderID);
	glLinkProgram(shaderProgramID);
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512]{};
		glGetProgramInfoLog(shaderProgramID, 512, nullptr, infoLog);
		RX_ERROR("Failed to link shader program - {}", infoLog);
	}

	for (GLuint shaderID : shaderIDs)
		glDeleteShader(shaderID);
	return (bool)success;
}

GLenum Shader::GetShaderGLenum(ShaderType shaderType)
{
	switch (shaderType)
	{
	case ShaderType::Vertex:
		return GL_VERTEX_SHADER;
	case ShaderType::Fragment:
		return GL_FRAGMENT_SHADER;
	case ShaderType::Geometry:
		return GL_GEOMETRY_SHADER;
	default:
		RX_ASSERT(false, "Unsupported shaderType used: {}", (int)shaderType)
		return GL_FALSE;
	}
}

GLint Shader::GetUniformLocation(std::string const& name) const
{
	GLint loc = glGetUniformLocation(m_ShaderProgramID, name.c_str());
	if (loc == -1)
		RX_WARN("Failed to locate '{}' in shader.", name);
	return loc;
}
