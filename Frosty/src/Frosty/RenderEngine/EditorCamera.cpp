#include <fypch.hpp>
#include "EditorCamera.hpp"

#include "Frosty/Core/Application.hpp"
#include "Frosty/Core/InputManager.hpp"
#include "Frosty/Core/KeyCodes.h"
#include "Frosty/Core/MouseButtonCodes.h"

namespace Frosty
{
	void EditorCamera::Init(const EditorCameraProps & props)
	{
		auto& win = Application::Get().GetWindow();
		m_ProjectionMatrix = glm::perspective(glm::radians(props.FieldOfView), (float)(win.GetViewport().z / win.GetViewport().w), props.ClippingPlanes.x, props.ClippingPlanes.y);
		m_Position = props.Position;
		m_Rotation = props.Rotation;
		RecalculateMatrix();
	}

	void EditorCamera::OnUpdate()
	{
		if (InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_2))
		{
			if (m_ResetMouse)
			{
				m_LastX = InputManager::GetMouseX();
				m_LastY = InputManager::GetMouseY();
				m_ResetMouse = false;
			}

			// Movement
			float velocity = m_TranslationSpeed * Time::DeltaTime();
			if (InputManager::IsKeyPressed(FY_KEY_LEFT_SHIFT))
			{
				velocity *= 10.0f;
			}
			if (InputManager::IsKeyPressed(FY_KEY_W))
			{
				Translate(m_Front * velocity);
			}
			else if (InputManager::IsKeyPressed(FY_KEY_S))
			{
				Translate(m_Front * velocity * -1.0f);
			}
			if (InputManager::IsKeyPressed(FY_KEY_A))
			{
				Translate(glm::cross(m_Front, { 0.0f, 1.0f, 0.0f }) * velocity * -1.0f);
			}
			else if (InputManager::IsKeyPressed(FY_KEY_D))
			{
				Translate(glm::cross(m_Front, { 0.0f, 1.0f, 0.0f }) * velocity);
			}

			// Rotation
			float yawOffset = InputManager::GetMouseX() - m_LastX;
			float pitchOffset = InputManager::GetMouseY() - m_LastY;

			m_LastX = InputManager::GetMouseX();
			m_LastY = InputManager::GetMouseY();

			yawOffset *= m_RotationSpeed;
			pitchOffset *= m_RotationSpeed;

			m_Rotation.x += yawOffset;
			m_Rotation.y += pitchOffset;

			if (m_Rotation.y > 89.0f)
				m_Rotation.y = 89.0f;
			if (m_Rotation.y < -89.0f)
				m_Rotation.y = -89.0f;

			RecalculateMatrix();
		}
		if (InputManager::IsMouseButtonReleased(FY_MOUSE_BUTTON_2))
		{
			m_ResetMouse = true;
		}
	}

	void EditorCamera::Translate(const glm::vec3 & offset)
	{
		m_Position += offset;
	}

	void EditorCamera::RecalculateMatrix()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(m_Rotation.x)) * cos(glm::radians(m_Rotation.y));
		front.y = sin(glm::radians(m_Rotation.y));
		front.z = sin(glm::radians(m_Rotation.x)) * cos(glm::radians(m_Rotation.y));
		m_Front = glm::normalize(front);

		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, { 0.0f, 1.0f, 0.0f });
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}