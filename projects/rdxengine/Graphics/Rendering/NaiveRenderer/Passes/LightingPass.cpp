#include "LightingPass.h"
#include "ServiceLayer.h"

using namespace rdx;

namespace {
	std::vector<GLuint> indices{
	0, 1, 2, 2, 3, 0, // Front face
	4, 5, 6, 6, 7, 4, // Back face
	6, 5, 2, 2, 1, 6, // Bottom face
	0, 3, 4, 4, 7, 0, // Top face
	7, 6, 1, 1, 0, 7, // Left face
	3, 2, 5, 5, 4, 3  // Right face
	};
}

bool LightingPass::InitImpl()
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

	return true;
}

bool LightingPass::TerminateImpl()
{
	m_Shader.Terminate();
	return true;
}

void LightingPass::DrawImpl()
{
	auto pRenderer = static_cast<NaiveRenderer*>(ServiceLayer::RenderingSystem());

	glm::vec4 m_BackbufferColor{ 0.2f, 0.3f, 0.3f, 1.0f };
	glClearColor(m_BackbufferColor.r, m_BackbufferColor.g, m_BackbufferColor.b, m_BackbufferColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	std::vector<glm::mat4> xforms{};
	RX_ECS_VIEWEACH(TransformComponent)(
		[&xforms](EntityID eid, TransformComponent& xform)
		{
			// glm::mat4 const& modelXform = xform.GetTransformMatrix();
			glm::mat4 modelXform = glm::translate(glm::mat4{ 1.f }, xform.Position);

			xforms.emplace_back(modelXform);
		});

	glNamedBufferSubData(pRenderer->m_DefaultMesh.m_VBOs[2],
		0,
		xforms.size() * sizeof(glm::mat4),
		(void*)(xforms.data() /*+ offset*/)); // Offset is when you have to batch the instancing, in case you overflow the maxinstances

	pRenderer->m_DefaultMesh.Bind();
	m_Shader.Bind();
	m_Shader.SetUniformMatrix4f("viewMat", m_Camera->GetViewMatrix());
	m_Shader.SetUniformMatrix4f("projMat", m_Camera->GetProjMatrix());
	m_Shader.SetUniform3f("uDirectionalLight", glm::vec3{ -1.f, 0.f, -1.f });
	m_Shader.SetUniform4f("uAmbientLight", glm::vec4{ 1.f });

	glDrawElementsInstanced(
		GL_TRIANGLES,
		(GLsizei)indices.size(),
		GL_UNSIGNED_INT,
		nullptr,
		(GLsizei)xforms.size()
	);

	m_Shader.Unbind();
}