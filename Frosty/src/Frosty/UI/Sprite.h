#ifndef SPRITE_H
#define SPRITE_H

#include "Frosty/Core/Camera/Camera.hpp"
#include "Frosty/API/Transform.h"

namespace Frosty
{
	class Sprite
	{
	private:
		Transform m_Transform;
		unsigned int m_Texture;
		unsigned int m_QuadVbo;
		std::vector<glm::vec3> m_Quad{
			glm::vec3(-1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, -1.0f, 0.0f),
			glm::vec3(-1.0f, -1.0f, 0.0f)
		};
		glm::vec4 color;
	public:
		Sprite();
		~Sprite();

		bool InitSprite();

		unsigned int LoadTexture();

		
	};
}


#endif // !SPRITE_H