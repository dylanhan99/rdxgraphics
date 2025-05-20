#pragma once
struct GLFWwindow; struct GLFWcursor; struct HWND__; typedef HWND__* HWND;

class GLFWWindow : public BaseSingleton<GLFWWindow>
{
	RX_SINGLETON_DECLARATION(GLFWWindow);
public:
	static bool Init();
	static void Terminate();

	static void ToggleMinMaxWindow();
	static void SetWindowTitle(std::string const& title);
	static void CenterCursor();
	static void SetInvisibleCursor(bool b);

	static bool IsWindowShouldClose();
	static void SetWindowShouldClose();
	static bool IsFocused();
	static void MakeContextCurrent();

	inline static GLFWwindow* GetWindowPointer() { return g.m_pWindow; }

	static glm::ivec2 GetCursorPos();
	inline static int GetCursorPosX() { return GetCursorPos().x; }
	inline static int GetCursorPosY() { return GetCursorPos().y; }

	static glm::ivec2 GetWindowPos();
	inline static int GetWindowPosX() { return GetWindowPos().x; }
	inline static int GetWindowPosY() { return GetWindowPos().y; }

	static glm::ivec2 GetWindowDims();
	inline static int GetWindowDimsX() { return GetWindowDims().x; }
	inline static int GetWindowDimsY() { return GetWindowDims().y; }

	static void SetIsVSync(bool b);
	inline static bool& IsVSync() { return g.m_IsVSync; }

private:
	static void RegisterCallbacks();

private:
	GLFWwindow* m_pWindow{ nullptr };
	HWND m_pWindowHandle{};

public: // FRC could be another class, but i'd rather it jsut be together with window since it's a singleton anyway
	static void Update(std::function<void(double)> fnUpdate);

	inline static double GetDT() { return g.m_AccumulatedDT * g.m_TimeScale; }
	inline static double GetTimeScale() { return g.m_TimeScale; }
	inline static void SetTimeScale(double ts) { g.m_TimeScale = ts >= 0.0 ? ts : 0.0; }
	void static SetTargetFPS(uint32_t target);
	inline static uint32_t GetFPS() { return g.m_FPS; } // Get fps but it only returns a new value every 1 second
	inline static uint32_t GetFPSIntervaled() { return g.m_IntervalFPS; }

private:
	bool m_IsVSync{ false };

	// Framerate control
	// Previous and Current gameloop time
	double m_PrevTime{}, m_CurrTime{};

	// Frametime / Deltatime
	uint32_t m_TargetTicks{}, m_TargetFPS{};
	double m_TargetDT{}, m_AccumulatedDT{};

	// Misc
	uint32_t m_FPS{}, m_IntervalFPS{}; // Frames per second
	double m_TimeScale{ 1.0 }; // DT multiplier
	double m_IntervalTimer{ 0.0 }; // Time accumulator for GetFPSIntervaled
};