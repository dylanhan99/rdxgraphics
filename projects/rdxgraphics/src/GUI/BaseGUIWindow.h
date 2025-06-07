#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include "ECS/EntityManager.h"

#define RX_GUIWINDOW_DEC(Foo)															\
public:																					\
	inline Foo(std::string name, ImGuiWindowFlags flags) : BaseGUIWindow(name, flags) {}\
	void UpdateImpl(float dt) override;													\
private:

class BaseGUIWindow
{
public:
	BaseGUIWindow() = delete;
	~BaseGUIWindow() = default;

	inline BaseGUIWindow(std::string name, ImGuiWindowFlags flags)
		: m_Name(name), m_Flags(flags) {}

	void Update(float dt);
	virtual void UpdateImpl(float dt) = 0;

private:
	std::string m_Name{};
	ImGuiWindowFlags m_Flags{};
};