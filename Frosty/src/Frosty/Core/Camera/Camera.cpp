#include "fypch.hpp"
#include "Camera.hpp"
#include "Frosty/Core/Application.hpp"
#include "Frosty/Core/Time.hpp"

namespace Frosty
{
	Camera::Camera()
	{
		m_CameraData.AspRatio = Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight();
		m_CameraData.Projection = glm::perspective(m_CameraData.FoV, m_CameraData.AspRatio, m_CameraData.NearPlane, m_CameraData.FarPlane);
		m_CameraData.View = glm::lookAt(m_CameraTranslationData.Pos, m_CameraTranslationData.Pos + m_CameraTranslationData.LookAtVec, m_CameraTranslationData.UpVec);
		m_CameraTranslationData.CamSpeed = 2.0f * Time::DeltaTime();

	}

	Camera::~Camera()
	{
	}
	
	glm::mat4 Frosty::Camera::GetView()
	{
		return m_CameraData.View;
	}

	glm::mat4 Frosty::Camera::GetProjection()
	{
		return m_CameraData.Projection;
	}

	glm::mat4 Camera::GetViewProjection()
	{
		return m_CameraData.Projection * m_CameraData.View;
	}
	
	void Frosty::Camera::CameraPositionUpdate()
	{
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
		m_CameraTranslationData.CamSpeed = 2.0f * Time::DeltaTime();
		
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			{
				m_CameraTranslationData.CamSpeed = m_CameraTranslationData.CamSpeed * 3.0f;
			}
			m_CameraTranslationData.Pos += m_CameraTranslationData.CamSpeed * m_CameraTranslationData.LookAtVec;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			{
				m_CameraTranslationData.CamSpeed = m_CameraTranslationData.CamSpeed * 3.0f;
			}
			m_CameraTranslationData.Pos -= m_CameraTranslationData.CamSpeed * m_CameraTranslationData.LookAtVec;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			{
				m_CameraTranslationData.CamSpeed = m_CameraTranslationData.CamSpeed * 3.0f;
			}
			m_CameraTranslationData.Pos -= glm::normalize(glm::cross(m_CameraTranslationData.LookAtVec, m_CameraTranslationData.UpVec)) * m_CameraTranslationData.CamSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			{
				m_CameraTranslationData.CamSpeed = m_CameraTranslationData.CamSpeed * 3.0f;
			}
			m_CameraTranslationData.Pos += glm::normalize(glm::cross(m_CameraTranslationData.LookAtVec, m_CameraTranslationData.UpVec)) * m_CameraTranslationData.CamSpeed;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			double MousePosX = 0;
			double MousePosY = 0;

			if (m_CameraRotationData.MouseState == -1)
			{
				glfwSetCursorPos(window, Application::Get().GetWindow().GetWidth() / 2, Application::Get().GetWindow().GetHeight() / 2);
				m_CameraRotationData.LastX = Application::Get().GetWindow().GetWidth() / 2.0f;
				m_CameraRotationData.LastY = Application::Get().GetWindow().GetHeight() / 2.0f;
				m_CameraRotationData.MouseState = 1;
			}
			CameraRotationUpdate(MousePosX, MousePosY);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_CameraRotationData.MouseState = -1;
		}

		m_CameraData.View = glm::lookAt(m_CameraTranslationData.Pos, m_CameraTranslationData.Pos + m_CameraTranslationData.LookAtVec, m_CameraTranslationData.UpVec);
	}

	void Frosty::Camera::CameraRotationUpdate(double xpos, double ypos)
	{
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
		glfwGetCursorPos(window, &xpos, &ypos);

		float Sensitivity = 0.5f;
		float XOffset = 0.0f;
		float YOffset = 0.0f;

		if (m_CameraRotationData.MouseState != 1)
		{
			XOffset = (float)xpos - m_CameraRotationData.LastX;
			YOffset = m_CameraRotationData.LastY - (float)ypos;
			m_CameraRotationData.LastX = (float)xpos;
			m_CameraRotationData.LastY = (float)ypos;

			XOffset *= Sensitivity;
			YOffset *= Sensitivity;

			m_CameraRotationData.Yaw += XOffset;
			m_CameraRotationData.Pitch += YOffset;
		}
		m_CameraRotationData.MouseState = 0;

		if (m_CameraRotationData.Pitch > 89.0f)
		{
			m_CameraRotationData.Pitch = 89.0f;
		}
		if (m_CameraRotationData.Pitch < -89.0f)
		{
			m_CameraRotationData.Pitch = -89.0f;
		}

		m_CameraTranslationData.LookAtVec.x = cos(glm::radians(m_CameraRotationData.Yaw)) * cos(glm::radians(m_CameraRotationData.Pitch));
		m_CameraTranslationData.LookAtVec.y = sin(glm::radians(m_CameraRotationData.Pitch));
		m_CameraTranslationData.LookAtVec.z = sin(glm::radians(m_CameraRotationData.Yaw)) * cos(glm::radians(m_CameraRotationData.Pitch));
	}
}
