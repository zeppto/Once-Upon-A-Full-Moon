#ifndef SPRITE_H
#define SPRITE_H

#include "Frosty/Core/Camera/Camera.hpp"
#include "Frosty/API/Transform.h"

namespace Frosty
{
	struct Texture {
		unsigned int id;
		std::string name;
	};

	class Sprite
	{
	private:
		Transform m_Transform;
		Texture m_Texture;
		unsigned int m_QuadVbo;

		float vertices[9 * 6] =
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

		void setColor(glm::vec4 color);
		float* GetQuad();
		int GetSize();
		Texture GetTexure();
		Transform &GetTransform();
	};
}


#endif // !SPRITE_H