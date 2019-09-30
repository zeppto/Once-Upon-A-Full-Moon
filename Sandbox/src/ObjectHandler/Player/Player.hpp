#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "Frosty.h"
#include"PlayerController.hpp"
//#include "TestBoxModel.hpp"


	class Player
	{



	public:


	private:
		Frosty::TempRender m_renderData;
		Frosty::PlayerController m_PlayerController;

		glm::vec3 m_Pos = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_Scale = glm::vec3(1.0f,1.0f,1.0f);
		glm::vec3 m_Rotation = glm::vec3(0.0f, 0.0f, 0.0f);


	public:
		Player(glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
		~Player();


		void UpdatePlayer();

	private:

	};




#endif 
