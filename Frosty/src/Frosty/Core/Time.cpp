#include <fypch.hpp>
#include "Time.hpp"

#include <GLFW/glfw3.h>		// Use chrono instead

namespace Frosty
{
	float Time::m_DeltaTime = 0.0f;
	float Time::m_LastFrame = 0.0f;
	int Time::m_CurrentFPS = 0;
	int Time::m_LastFPS = 0;
	float Time::m_FPSResetCounter = 1.0f;
	int Time::m_FPSCounter = 0;
	std::stack<float> Time::m_Timers;
		
	void Time::StartTimer(const std::string& func)
	{
		FY_CORE_TRACE("Timer started for '{0}'!", func);
		m_Timers.emplace((float)glfwGetTime());
	}
	
	void Time::EndTimer(const std::string& func)
	{
		float currentTime = (float)glfwGetTime();
		float resultTime = currentTime - m_Timers.top();
		m_Timers.pop();
	
		FY_CORE_TRACE("Timer for '{0}' stopped at {1} seconds.", func, resultTime);
	}

	void Time::OnUpdate()
	{
		if (m_FPSCounter >= MAX_FPS_COUNTER)
		{
			m_FPSCounter = 0;
		}
		float currentFrame = (float)glfwGetTime();
		m_DeltaTime = currentFrame - m_LastFrame;
		m_LastFrame = currentFrame;

		// FPS
		m_FPSResetCounter -= m_DeltaTime;
		if (m_FPSResetCounter <= 0.0f)
		{
			m_LastFPS = m_CurrentFPS;
			m_CurrentFPS = 0;
			m_FPSResetCounter = 1.0f;
		}
		else
		{
			m_CurrentFPS += 1;
		}
		m_FPSCounter++;
	}
}