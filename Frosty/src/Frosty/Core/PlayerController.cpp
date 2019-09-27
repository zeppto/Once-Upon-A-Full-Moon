#include "fypch.hpp"
#include "PlayerController.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "InputManager.hpp"
namespace Frosty
{

	PlayerController::PlayerController()
	{
	}
	PlayerController::~PlayerController()
	{
	}

	glm::vec3 PlayerController::PlayerControllerUpdate()
	{
		InputManager myImputManager;
		glm::vec3 movment(0.0f);
		m_CurrentFrame = glfwGetTime();
		m_DeltaTime = m_CurrentFrame - m_LastFrame;
		m_LastFrame = m_CurrentFrame;
		float speed = m_Speed * m_DeltaTime;

		if (myImputManager.IsKeyPressed(GLFW_KEY_W))
		{
			movment += speed * glm::vec3(0.0f, 1.5f, 0.0f);
		}
		if (myImputManager.IsKeyPressed(GLFW_KEY_S))
		{
			movment -= speed * glm::vec3(0.0f, 1.5f, 0.0f);
		}
		if (myImputManager.IsKeyPressed(GLFW_KEY_A))
		{
			movment -= speed * glm::vec3(1.5f, 0.0f, 0.0f);
		}
		if (myImputManager.IsKeyPressed(GLFW_KEY_D))
		{
			movment += speed * glm::vec3(1.5f, 0.0f, 0.0f);
		}
		//ta port sen
		if (myImputManager.IsKeyPressed(GLFW_KEY_DOWN))
		{
			movment -= speed * glm::vec3(0.0f, 0.0f, 1.5f);
		}
		if (myImputManager.IsKeyPressed(GLFW_KEY_UP))
		{
			movment += speed * glm::vec3(0.0f, 0.0f, 1.5f);
		}
		return movment;
	}
}
