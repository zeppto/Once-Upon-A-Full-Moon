#include "fypch.hpp"
#include "PlayerController.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "Frosty/Core/InputManager.hpp"

namespace Frosty
{
	PlayerController::PlayerController()
	{
	}
	PlayerController::~PlayerController()
	{
	}

	glm::vec3 PlayerController::PlayerControllerMovement()
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
	Attacks PlayerController::PlayerControllerAttacks()
	{
		Attacks myAttack;
		InputManager myImputManager;
		m_EnemyCooldown -= m_DeltaTime;
		m_PlayerCooldown -= m_DeltaTime;
		if (myImputManager.IsKeyPressed(GLFW_KEY_ENTER))
		{
			myAttack.enemyIsAttacking = true;
			if (m_EnemyCooldown < 0)
			{
				myAttack.enemyCanAttack = true;
				m_EnemyCooldown = 0.5f;
			}
			else
				myAttack.enemyCanAttack = false;
		}

		if (myImputManager.IsKeyPressed(GLFW_KEY_SPACE))
		{
			myAttack.playerIsAttacking = true;
			if (m_PlayerCooldown < 0)
			{
				myAttack.playerCanAttack = true;
				m_PlayerCooldown = 0.5f;
			}
			else
				myAttack.playerCanAttack = false;
		}

		return myAttack;
	}

}