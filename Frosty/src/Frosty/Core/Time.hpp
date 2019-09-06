#ifndef TIME_HPP
#define TIME_HPP

namespace Frosty
{
	class Time
	{
	public:
		virtual ~Time() { }

		static Time& Get();
		inline float DeltaTime() { return m_DeltaTime; }
		inline int FPS() { return m_LastFPS; }
		inline int GetFrameCount() {return m_FPSCounter; }
		void StartTimer(const std::string& func);
		void EndTimer(const std::string& func);

		void OnUpdate();

	private:
		Time();
		inline static void Delete() { delete s_Instance; }

	public:
		static const int MAX_FPS_COUNTER = 10000;

	private:
		float m_DeltaTime = 0.0f;
		float m_LastFrame = 0.0f;
		int m_CurrentFPS;
		int m_LastFPS;
		float m_FPSResetCounter = 1.0f;
		int m_FPSCounter = 0;
		std::stack<float> m_Timers;

		static Time* s_Instance;
		friend class Application;
	};
}

#endif // !TIME_HPP
