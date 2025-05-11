#pragma once

class BaseGUIWindow
{
public:
	BaseGUIWindow() = delete;
	~BaseGUIWindow() = default;

	inline BaseGUIWindow(std::string name, int flags)
		: m_Name(name), m_Flags(flags) {}

	void Update(float dt);
	virtual void UpdateImpl(float dt) = 0;

private:
	std::string m_Name{};
	int m_Flags{};
};