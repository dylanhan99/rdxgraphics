#pragma once

class BaseGUIWindow
{
public:
	BaseGUIWindow() = delete;
	~BaseGUIWindow() = default;

	inline BaseGUIWindow(std::string name, ImGuiWindowFlags flags)
		: m_Name(name), m_Flags(flags) {}

	void Update(float dt);
	virtual void Update(float dt) = 0;

private:
	std::string m_Name{};
	ImGuiWindowFlags m_Flags{};
};