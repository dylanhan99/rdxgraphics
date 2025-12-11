#include "ConsolidationPass.h"
#include "ServiceLayer.h"

using namespace rdx;

namespace {
	std::vector<GLuint> indices{
		0, 1, 2,
		2, 3, 0
	};
}

bool ConsolidationPass::InitImpl()
{
	{
		const char* vertexShaderSource =
			"#version 450 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"layout (location = 1) in vec3 aNormal;\n"
			"layout (location = 2) in mat4 aXform;\n"
			"uniform mat4 viewMat;\n"
			"uniform mat4 projMat;\n"
			"out VS_OUT\n"
			"{\n"
			"	vec3 Position;\n"
			"	vec3 Normal;\n"
			"	flat vec4 DiffuseColor;\n"
			"} vs_out;\n"
			"\n"
			"void main()\n"
			"{\n"
			"   vec4 model = aXform * vec4(aPos, 1.0);\n"
			"   vs_out.Position = model.xyz;\n"
			"	vs_out.Normal = mat3(transpose(inverse(aXform))) * aNormal;\n"
			"   vs_out.DiffuseColor = vec4(1.f, 0.f, 1.f, 1.f);\n"
			"\n"
			"   gl_Position = projMat * viewMat * model;\n"
			"}\0";

		const char* fragmentShaderSource =
			"#version 450 core\n"
			"uniform vec3 uDirectionalLight;\n"
			"uniform vec4 uAmbientLight;\n"
			"out vec4 FragColor;\n"
			"in VS_OUT\n"
			"{\n"
			"	vec3 Position;\n"
			"	vec3 Normal;\n"
			"	flat vec4 DiffuseColor;\n"
			"} fs_in;\n"
			"void main()\n"
			"\n"
			"{\n"
			"	vec3 fragPos = fs_in.Position;\n"
			"	vec3 norm = normalize(fs_in.Normal);\n"
			"	vec3 lightColor = vec3(1.0);\n"
			"	vec3 lightDir = normalize(-uDirectionalLight);\n"
			"\n"
			"	vec3 ambient = uAmbientLight.xyz * uAmbientLight.w;\n"
			"\n"
			"	float diff = max(dot(norm, lightDir), 0.0);\n"
			"	vec3 diffuse = diff * lightColor;\n"
			"\n"
			"	vec3 specular = vec3(0.0);\n"
			"\n"
			"	vec3 result = ambient + (diffuse + specular) * (fs_in.DiffuseColor.xyz * fs_in.DiffuseColor.w);\n"
			"	FragColor = vec4(result, 1.0);\n"
			"}\0";

		m_Shader.Init({
			{NaiveRenderer::Shader::Type::Vertex, vertexShaderSource},
			{NaiveRenderer::Shader::Type::Fragment, fragmentShaderSource}
			});
	}

	{
		std::vector<glm::vec2> vertices{
			{ -1.0f,  1.0f },
			{ -1.0f, -1.0f },
			{  1.0f, -1.0f },
			{  1.0f,  1.0f }
		};
		std::vector<glm::vec2> texCoords{
			{ 0.f, 1.f },
			{ 0.f, 0.f },
			{ 1.f, 0.f },
			{ 1.f, 1.f }
		};

		NaiveRenderer::Mesh::VertexLayout layout{};
		layout.Push(NaiveRenderer::Mesh::VertexAttribute{
			.AttributeType = NaiveRenderer::Mesh::VertexAttributeType::Vec2,
			.AttributeCount = 1,
			.FundamentalType = NaiveRenderer::Mesh::VertexAttributeType::Float,
			.FundamentalCount = 2,
			.Data = vertices.data(),
			.Length = vertices.size(),
			.IsInstanced = false,
			.IsNormalized = false
			});
		layout.Push(NaiveRenderer::Mesh::VertexAttribute{
			.AttributeType = NaiveRenderer::Mesh::VertexAttributeType::Vec2,
			.AttributeCount = 1,
			.FundamentalType = NaiveRenderer::Mesh::VertexAttributeType::Float,
			.FundamentalCount = 2,
			.Data = texCoords.data(),
			.Length = texCoords.size(),
			.IsInstanced = false,
			.IsNormalized = false
			});
		m_ScreenMesh.Init(layout, indices);
	}

	return true;
}

bool ConsolidationPass::TerminateImpl()
{
	m_ScreenMesh.Terminate();
	m_Shader.Terminate();
	return true;
}