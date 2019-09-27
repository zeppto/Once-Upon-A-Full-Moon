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
		unsigned int m_TextureID;
		unsigned int m_QuadVbo;

		float vertices[9*6] =
		{
			-1.0f, -1.0f, 0.0f, 0.8f, 0.0f, 0.8f, 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f, 1.0f, -1.0f,
			 1.0f, 1.0f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f, 1.0f, 1.0f,
			 1.0f, 1.0f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f, 1.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f, -1.0f, 1.0f,
			 -1.0f, -1.0f, 0.0f, 0.8f, 0.0f, 0.8f, 1.0f, -1.0f, -1.0f
		};

		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	public:
		Sprite();
		~Sprite();

		bool Init();

		unsigned int LoadTexture();
		
		float *GetQuad();
		int GetSize();
		unsigned int GetTexureID();
	};
}


#endif // !SPRITE_H