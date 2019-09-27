#pragma once

namespace Frosty
{
	class OrtoCamera
	{
	public:
		OrtoCamera(float left, float right, float bottom, float top);
		
		const glm::vec3& GetPosition()const;
		float GetRotation()const;

		void SetPosition(const glm::vec3& position);
		void SetRotation(float rotation);

		const glm::mat4& GetProjectionMatrix()const;
		const glm::mat4& GetViewMatrix()const;
		const glm::mat4& GetViewProjectionMatrix()const;
	private:
		void UpdateViewMatrix();
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position;
		float m_Rotation = 0.0f;
	};
}
