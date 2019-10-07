#include "fypch.hpp"
#include "CameraManager.hpp"
#include "Frosty/Core/Application.hpp"
#include "Frosty/Core/Time.hpp"

namespace Frosty
{
	CameraManager::CameraManager()		
	{
		m_Camera = new Camera();
	}

	CameraManager::~CameraManager()
	{
		if (m_Camera != nullptr)
		{
			delete m_Camera;
		}
	}
	
	void CameraManager::CameraPositionUpdate()
	{
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
		m_Camera->m_CameraTranslationData.CamSpeed = 2.0f * Time::DeltaTime();

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			{
				m_Camera->m_CameraTranslationData.CamSpeed = m_Camera->m_CameraTranslationData.CamSpeed * 3.0f;
			}
			m_Camera->m_CameraTranslationData.Pos += m_Camera->m_CameraTranslationData.CamSpeed * m_Camera->m_CameraTranslationData.LookAtVec;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			{
				m_Camera->m_CameraTranslationData.CamSpeed = m_Camera->m_CameraTranslationData.CamSpeed * 3.0f;
			}
			m_Camera->m_CameraTranslationData.Pos -= m_Camera->m_CameraTranslationData.CamSpeed * m_Camera->m_CameraTranslationData.LookAtVec;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			{
				m_Camera->m_CameraTranslationData.CamSpeed = m_Camera->m_CameraTranslationData.CamSpeed * 3.0f;
			}
			m_Camera->m_CameraTranslationData.Pos -= glm::normalize(glm::cross(m_Camera->m_CameraTranslationData.LookAtVec, m_Camera->m_CameraTranslationData.UpVec)) * m_Camera->m_CameraTranslationData.CamSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			{
				m_Camera->m_CameraTranslationData.CamSpeed = m_Camera->m_CameraTranslationData.CamSpeed * 3.0f;
			}
			m_Camera->m_CameraTranslationData.Pos += glm::normalize(glm::cross(m_Camera->m_CameraTranslationData.LookAtVec, m_Camera->m_CameraTranslationData.UpVec)) * m_Camera->m_CameraTranslationData.CamSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			{
				m_Camera->m_CameraTranslationData.CamSpeed = m_Camera->m_CameraTranslationData.CamSpeed * 3.0f;
			}
			m_Camera->m_CameraTranslationData.Pos -= glm::normalize(m_Camera->m_CameraTranslationData.UpVec) * m_Camera->m_CameraTranslationData.CamSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			{
				m_Camera->m_CameraTranslationData.CamSpeed = m_Camera->m_CameraTranslationData.CamSpeed * 3.0f;
			}
			m_Camera->m_CameraTranslationData.Pos += glm::normalize(m_Camera->m_CameraTranslationData.UpVec) * m_Camera->m_CameraTranslationData.CamSpeed;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			double MousePosX = 0;
			double MousePosY = 0;

			if (m_Camera->m_CameraRotationData.MouseState == -1)
			{
				glfwSetCursorPos(window, Application::Get().GetWindow().GetWidth() / 2, Application::Get().GetWindow().GetHeight() / 2);
				m_Camera->m_CameraRotationData.LastX = Application::Get().GetWindow().GetWidth() / 2.0f;
				m_Camera->m_CameraRotationData.LastY = Application::Get().GetWindow().GetHeight() / 2.0f;
				m_Camera->m_CameraRotationData.MouseState = 1;
			}
			CameraRotationUpdate(MousePosX, MousePosY);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_Camera->m_CameraRotationData.MouseState = -1;
		}

		m_Camera->m_CameraData.View = glm::lookAt(m_Camera->m_CameraTranslationData.Pos, m_Camera->m_CameraTranslationData.Pos + m_Camera->m_CameraTranslationData.LookAtVec, m_Camera->m_CameraTranslationData.UpVec);
	}

	void CameraManager::CameraRotationUpdate(double xpos, double ypos)
	{
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
		glfwGetCursorPos(window, &xpos, &ypos);

		float Sensitivity = 0.5f;
		float XOffset = 0.0f;
		float YOffset = 0.0f;

		if (m_Camera->m_CameraRotationData.MouseState != 1)
		{
			XOffset = (float)xpos - m_Camera->m_CameraRotationData.LastX;
			YOffset = m_Camera->m_CameraRotationData.LastY - (float)ypos;
			m_Camera->m_CameraRotationData.LastX = (float)xpos;
			m_Camera->m_CameraRotationData.LastY = (float)ypos;

			XOffset *= Sensitivity;
			YOffset *= Sensitivity;

			m_Camera->m_CameraRotationData.Yaw += XOffset;
			m_Camera->m_CameraRotationData.Pitch += YOffset;
		}
		m_Camera->m_CameraRotationData.MouseState = 0;

		if (m_Camera->m_CameraRotationData.Pitch > 89.0f)
		{
			m_Camera->m_CameraRotationData.Pitch = 89.0f;
		}
		if (m_Camera->m_CameraRotationData.Pitch < -89.0f)
		{
			m_Camera->m_CameraRotationData.Pitch = -89.0f;
		}

		m_Camera->m_CameraTranslationData.LookAtVec.x = cos(glm::radians(m_Camera->m_CameraRotationData.Yaw)) * cos(glm::radians(m_Camera->m_CameraRotationData.Pitch));
		m_Camera->m_CameraTranslationData.LookAtVec.y = sin(glm::radians(m_Camera->m_CameraRotationData.Pitch));
		m_Camera->m_CameraTranslationData.LookAtVec.z = sin(glm::radians(m_Camera->m_CameraRotationData.Yaw)) * cos(glm::radians(m_Camera->m_CameraRotationData.Pitch));
	}	
}