#ifndef TESTBOXMODEL_HPP
#define TESTBOXMODEL_HPP
#include "Frosty/Core/PlayerController.hpp"

namespace Frosty
{
	class TestBoxModel
	{
	public:
		TestBoxModel(glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
		~TestBoxModel();

		void SetUp();
		void Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

		void SetShaderProgram(unsigned int shaderProgram);
		void SetColor(glm::vec3 color);
		void SetIsPlayer(bool isPlayer);
		void SetPosition(glm::vec3 pos);

		void Update(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
		void Update();


	private:
		glm::vec3 m_Color, m_Pos, m_Rotation, m_Scale, m_Max, m_Min;
		unsigned int m_TestBoxVBO, m_ShaderProgram;
		glm::mat4 m_World;
		bool m_IsPlayer;
		PlayerController m_PlayerController;
	};
}

#endif