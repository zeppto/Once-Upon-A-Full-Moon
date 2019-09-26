#include "fypch.hpp"
#include "Camera.hpp"
#include "Frosty/Core/Application.hpp"

namespace Frosty
{
	Frosty::Camera::Camera()
	{
		m_CameraData.AspRatio = Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight();
		m_CameraData.LastX = Application::Get().GetWindow().GetWidth() / 2.0f;
		m_CameraData.LastY = Application::Get().GetWindow().GetHeight() / 2.0f;
		m_CameraData.Projection = glm::perspective(m_CameraData.FoV, m_CameraData.AspRatio, m_CameraData.NearPlane, m_CameraData.FarPlane);
		m_CameraData.View = glm::lookAt(m_CameraData.Pos, m_CameraData.Pos + m_CameraData.LookAtVec, m_CameraData.UpVec);
	}

	Frosty::Camera::~Camera()
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
	void Frosty::Camera::CameraPositionUpdate()
	{
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		m_CameraData.CurrentFrame = (float)glfwGetTime();
		m_CameraData.DeltaTime = m_CameraData.CurrentFrame - m_CameraData.LastFrame;
		m_CameraData.LastFrame = m_CameraData.CurrentFrame;
		m_CameraData.CamSpeed = 2.0f * m_CameraData.DeltaTime;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			{
				m_CameraData.CamSpeed = m_CameraData.CamSpeed * 3.0f;
			}
			m_CameraData.Pos += m_CameraData.CamSpeed * m_CameraData.LookAtVec;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			{
				m_CameraData.CamSpeed = m_CameraData.CamSpeed * 3.0f;
			}
			m_CameraData.Pos -= m_CameraData.CamSpeed * m_CameraData.LookAtVec;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			{
				m_CameraData.CamSpeed = m_CameraData.CamSpeed * 3.0f;
			}
			m_CameraData.Pos -= glm::normalize(glm::cross(m_CameraData.LookAtVec, m_CameraData.UpVec)) * m_CameraData.CamSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			{
				m_CameraData.CamSpeed = m_CameraData.CamSpeed * 3.0f;
			}
			m_CameraData.Pos += glm::normalize(glm::cross(m_CameraData.LookAtVec, m_CameraData.UpVec)) * m_CameraData.CamSpeed;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			if (m_CameraData.MouseState == -1)
			{
				glfwSetCursorPos(window, Application::Get().GetWindow().GetWidth() / 2, Application::Get().GetWindow().GetHeight() / 2);
				m_CameraData.LastX = Application::Get().GetWindow().GetWidth() / 2.0f;
				m_CameraData.LastY = Application::Get().GetWindow().GetHeight() / 2.0f;
				m_CameraData.MouseState = 1;
			}
			CameraRotationUpdate(m_CameraData.MousePosX, m_CameraData.MousePosY);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_CameraData.MouseState = -1;
		}

		m_CameraData.View = glm::lookAt(m_CameraData.Pos, m_CameraData.Pos + m_CameraData.LookAtVec, m_CameraData.UpVec);
	}
	void Frosty::Camera::CameraRotationUpdate(double xpos, double ypos)
	{
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
		glfwGetCursorPos(window, &xpos, &ypos);

		if (m_CameraData.MouseState != 1)
		{
			m_CameraData.XOffset = (float)xpos - m_CameraData.LastX;
			m_CameraData.YOffset = m_CameraData.LastY - (float)ypos;
			m_CameraData.LastX = (float)xpos;
			m_CameraData.LastY = (float)ypos;

			m_CameraData.XOffset *= m_CameraData.Sensitivity;
			m_CameraData.YOffset *= m_CameraData.Sensitivity;

			m_CameraData.Yaw += m_CameraData.XOffset;
			m_CameraData.Pitch += m_CameraData.YOffset;
		}
		m_CameraData.MouseState = 0;

		if (m_CameraData.Pitch > 89.0f)
		{
			m_CameraData.Pitch = 89.0f;
		}
		if (m_CameraData.Pitch < -89.0f)
		{
			m_CameraData.Pitch = -89.0f;
		}

		m_CameraData.LookAtVec.x = cos(glm::radians(m_CameraData.Yaw)) * cos(glm::radians(m_CameraData.Pitch));
		m_CameraData.LookAtVec.y = sin(glm::radians(m_CameraData.Pitch));
		m_CameraData.LookAtVec.z = sin(glm::radians(m_CameraData.Yaw)) * cos(glm::radians(m_CameraData.Pitch));	
	}
}
