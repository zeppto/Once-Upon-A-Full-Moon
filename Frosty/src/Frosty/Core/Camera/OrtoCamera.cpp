#include "fypch.hpp"
#include "OrtoCamera.hpp"

namespace Frosty
{
	OrtoCamera::OrtoCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	const glm::vec3 & OrtoCamera::GetPosition() const
	{
		return m_Position;
	}

	float OrtoCamera::GetRotation() const
	{
		return m_Rotation;
	}

	void OrtoCamera::SetPosition(const glm::vec3 & position)
	{
		m_Position = position;
		UpdateViewMatrix();
	}

	void OrtoCamera::SetRotation(float rotation)
	{
		m_Rotation = rotation;
		UpdateViewMatrix();
	}

	const glm::mat4 & OrtoCamera::GetProjectionMatrix() const
	{
		return m_ProjectionMatrix;
	}

	const glm::mat4 & OrtoCamera::GetViewMatrix() const
	{
		return m_ViewMatrix;
	}

	const glm::mat4 & OrtoCamera::GetViewProjectionMatrix() const
	{
		return m_ViewProjectionMatrix;
	}

	void OrtoCamera::UpdateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) 
			* glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}
