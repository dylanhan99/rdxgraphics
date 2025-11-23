#ifndef NAIVERENDERER_H
#define NAIVERENDERER_H
#include "Graphics/BaseRenderer.h"
#include <gl/glew.h>

namespace rdx
{
	class NaiveRenderer : public BaseRenderer
	{
		class Shader
		{
		public:
			enum class Type {
				Vertex,
				Fragment,
				Geometry,
				Compute,

				MAX
			};
			inline static const std::array<const char*, (size_t)Type::MAX> SHADERTYPE_STRINGS{
				"Vertex",
				"Fragment",
				"Geometry",
				"Compute"
			};

		public:
			bool Init(std::initializer_list<std::pair<Type, std::string>> shaderList);
			bool Terminate();

			void Bind() const;
			void Unbind() const;

			//template <typename T>
			//SetUniform(std::string_view name, )

			void SetUniform1i(std::string_view const& name, bool v);
			void SetUniform1i(std::string_view const& name, int const& v);
			void SetUniform3f(std::string_view const& name, glm::vec3 const& v);
			void SetUniform3fv(std::string_view const& name, std::vector<glm::vec3> const& v);
			void SetUniform4f(std::string_view const& name, glm::vec4 const& v);
			void SetUniformMatrix4f(std::string_view const& name, glm::mat4 const& v);
			void SetUniformMatrix4fv(std::string_view const& name, std::vector<glm::mat4> const& v);

			inline bool IsValid() const { return *this; }
			inline operator bool() const { return m_ProgramID != 0; }

		private:
			GLint GetUniformLocation(std::string_view const& name) const;
			
			GLenum TranslateShaderType(Type const t);
			GLuint LoadShader(GLenum shaderType, std::string const& buffer);
			bool LinkShaderProgram(std::vector<GLuint> const& shaderIDs); // Assumes all ids are valid
			void CleanupShaders(std::vector<GLuint> const& shaderIDs);

		private:
			GLuint m_ProgramID{};
		};
		class VertexArrayObject
		{
		public:
			bool Init(std::vector<GLuint> indices = {});

		private:
			GLuint m_ID;
		};

	public:
		bool InitImpl() override;
		bool TerminateImpl() override;


	private:
		void DrawImpl() override;

	private:
		Shader m_DefaultShader{};
	};
}

#endif