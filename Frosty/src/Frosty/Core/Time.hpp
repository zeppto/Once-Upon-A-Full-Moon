#ifndef TIME_HPP
#define TIME_HPP

namespace Frosty
{
	class Time
	{
	protected:
		Time() = default;
	public:
		Time(const Time&) = delete;
		Time& operator=(const Time&) = delete;

		static inline float DeltaTime() { return m_DeltaTime; }
		static inline int FPS() { return m_LastFPS; }
		static inline int GetFrameCount() {return m_FPSCounter; }
		static void StartTimer(const std::string& func);
		static void EndTimer(const std::string& func);

		static void OnUpdate();
	public:
		static const int MAX_FPS_COUNTER = 10000;
	private:
		static float m_DeltaTime;
		static float m_LastFrame;
		static int m_CurrentFPS;
		static int m_LastFPS;
		static float m_FPSResetCounter;
		static int m_FPSCounter;
		static std::stack<float> m_Timers;
	};
}
#endif
