#pragma once

class FramerateController : BaseSingleton<FramerateController>
{
	RX_SINGLETON_DECLARATION(FramerateController);
public:
	static void Init(uint32_t targetFPS);
	static void Update(std::function<void(double)> fnUpdate);
	static void StartGameLoop();
	static void StartFrame();

	inline static double GetDT() { return g.m_AccumulatedDT * g.m_TimeScale; }

	inline static double GetTimeScale() { return g.m_TimeScale; }
	inline static void SetTimeScale(double ts) { g.m_TimeScale = ts >= 0.0 ? ts : 0.0; }

	void static SetTargetFPS(uint32_t target);
	inline static uint32_t GetFPS() { return g.m_FPS; } // Get fps but it only returns a new value every 1 second
	inline static uint32_t GetFPSIntervaled() { return g.m_IntervalFPS; }

private:
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