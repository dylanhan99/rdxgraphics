#include <pch.h>
#include "RenderSystem.h"

#include "GLFWWindow/GLFWWindow.h"

RX_SINGLETON_EXPLICIT(RenderSystem);

bool RenderSystem::Init()
{
	if (!gladLoadGL(glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void RenderSystem::Terminate()
{
}

void RenderSystem::Update(double dt)
{
	RX_UNREF_PARAM(dt);

	glClearColor(g.m_BackColor.x, g.m_BackColor.y, g.m_BackColor.z, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
