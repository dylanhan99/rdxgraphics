#ifndef NAIVERENDERER_H
#define NAIVERENDERER_H
#include "Graphics/Rendering/BaseRenderer.h"
#include "Graphics/MultiPassing/RenderPipeline.h"
#include <gl/glew.h>

namespace rdx
{
	class NaiveRenderer : public BaseRenderer
	{
	public:
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
			inline operator bool() const { return m_ProgramID > 0; }

		private:
			GLint GetUniformLocation(std::string_view const& name) const;
			
			GLenum TranslateShaderType(Type const t);
			GLuint LoadShader(GLenum shaderType, std::string const& buffer);
			bool LinkShaderProgram(std::vector<GLuint> const& shaderIDs); // Assumes all ids are valid
			void CleanupShaders(std::vector<GLuint> const& shaderIDs);

		private:
			GLuint m_ProgramID{};
		};
		class Mesh // Structure of Arrays (SOA)
		{
		public:
			enum class VertexAttributeType
			{
				Vec2, Vec3, Vec4,
				UInt8, UInt32, Float,
			};

			struct VertexAttribute
			{
				//uint32_t Location{}; // Shader location

				VertexAttributeType AttributeType{}; // eg vec4
				uint32_t AttributeCount{};			 // Most would be 1, but say for Mat4, it has to be made of 4 vec4

				VertexAttributeType FundamentalType{}; // eg float
				uint32_t FundamentalCount{}; // Would be 4 floats for vec4

				void* Data{};
				size_t Length{}; // Array size

				bool IsInstanced{};
				bool IsNormalized{};
			};

			struct VertexLayout
			{
				inline void Push(VertexAttribute attrib) { Attributes.emplace_back(std::move(attrib)); }

				std::vector<VertexAttribute> Attributes{};
			};

		public:
			bool Init(VertexLayout const& layout, std::vector<GLuint> indices = {});
			bool Terminate();

			void Bind() const;

			inline bool IsValid() const { return *this; }
			inline operator bool() const { return m_VAO > 0; }

			std::vector<GLuint> m_VBOs; // temp

		private:
			size_t GetFundamentalSize(VertexAttributeType const t);
			GLenum TraslateAttribType(VertexAttributeType const t);

		private:
			GLuint m_VAO;
		};

	public:
		bool InitImpl() override;
		bool TerminateImpl() override;

		void SetDepthTest(bool flag) override { if (flag) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST); }

		inline void RegisterPipeline(RenderPipeline pipeline)
		{
			m_Pipelines.emplace_back(std::move(pipeline));
		}

	private:
		void DrawImpl() override;

	public: // temp public
		Shader m_DefaultShader{};
		Mesh m_DefaultMesh{};
		
	private:
		std::vector<RenderPipeline> m_Pipelines{};
	};
}

#endif