#include "TestPass.h"
#include <gl/glew.h>

#include "ServiceLayer.h"
#include "Graphics/Rendering/NaiveRenderer/NaiveRenderer.h"

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

void TestPass::DrawImpl()
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

	glNamedBufferSubData(pRenderer->m_DefaultMesh.m_VBOs[1],
		0,
		xforms.size() * sizeof(glm::mat4),
		(void*)(xforms.data() /*+ offset*/)); // Offset is when you have to batch the instancing, in case you overflow the maxinstances

	pRenderer->m_DefaultMesh.Bind();
	pRenderer->m_DefaultShader.Bind();
	pRenderer->m_DefaultShader.SetUniformMatrix4f("viewMat", pRenderer->m_EditorCamera.GetViewMatrix());
	pRenderer->m_DefaultShader.SetUniformMatrix4f("projMat", pRenderer->m_EditorCamera.GetProjMatrix());

	glDrawElementsInstanced(
		GL_TRIANGLES,
		(GLsizei)indices.size(),
		GL_UNSIGNED_INT,
		nullptr,
		(GLsizei)xforms.size()
	);
	pRenderer->m_DefaultShader.Unbind();
}