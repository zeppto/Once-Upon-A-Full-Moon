#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "Frosty/Core/TestBoxModel.hpp"

namespace Frosty
{
	class Player : public TestBoxModel
	{
	public:
		Player(glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
		~Player();

	private:

	};
}



#endif 
