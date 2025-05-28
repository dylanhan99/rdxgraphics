#pragma once
#include "GraphicsCommon.h"

enum ShaderType : int {
	Vertex,
	Fragment,
	Geometry,

	MAX
};

class Shader
{
public:
	// This init does not terminate any preexisting shaders. Be wary. Use reload if that's what you're intending to do.
	bool Init(std::vector<std::pair<ShaderType, std::string>> const& shaderAssets);
	bool Init(std::vector<std::pair<ShaderType, std::filesystem::path>> const& shaderAssets);
	void Terminate();
	bool Reload(); // Actually will terminate before re-initializing

	void Bind() const;

	void SetUniform1i(std::string const& name, bool v);
	void SetUniform1i(std::string const& name, int const& v);
	void SetUniform3f(std::string const& name, glm::vec3 const& v);
	void SetUniform3fv(std::string const& name, std::vector<glm::vec3> const& v);
	void SetUniform4f(std::string const& name, glm::vec4 const& v);
	void SetUniformMatrix4f(std::string const& name, glm::mat4 const& v);
	void SetUniformMatrix4fv(std::string const& name, std::vector<glm::mat4> const& v);

private:
	// shaderPath is appended to g_WorkingDir. eg. ".../rdxgraphics" / "Assets/default.vert" = .../rdxgraphics/Assets/default.vert
	static GLuint LoadShader(GLenum shaderType, std::filesystem::path const& shaderPath);
	static GLuint LoadShader(GLenum shaderType, std::string const& shaderBuffer);
	static bool LinkAndValidateShaderProgram(std::vector<GLuint>& shaderIDs, GLuint& shaderProgramID);
	static GLenum GetShaderGLenum(ShaderType shaderType);
	GLint GetUniformLocation(std::string const& name) const;

public:
	GLuint m_ShaderProgramID{};

	// fs::path is absolute
	std::array<std::filesystem::path, (size_t)ShaderType::MAX> m_ShaderAssets{};
};