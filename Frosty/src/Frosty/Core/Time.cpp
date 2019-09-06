#include <fypch.hpp>
#include "Time.hpp"

#include <GLFW/glfw3.h>

namespace Frosty
{
	Time* Time::s_Instance = nullptr;

	Time::Time()
	{
		m_LastFrame = (float)glfwGetTime();
	}

	Time & Time::Get()
	{
		if (s_Instance == nullptr)
		{
			// Initiate
			s_Instance = new Time();
		}

		return *s_Instance;
	}
	
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