#ifndef SPRITE_H
#define SPRITE_H

#include "Frosty/API/Transform.h"

namespace Frosty
{
	struct Texture {
		unsigned int id;
		std::string name;
	};

	class Sprite
	{

	public:
		float vertices[9 * 6] =
		{
			-1.0f, -1.0f, 0.0f, 0.8f, 0.0f, 0.8f, 1.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f, 1.0f, 0.0f,
			 1.0f, 1.0f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f, 1.0f, 1.0f,
			 1.0f, 1.0f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f, 1.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f, 0.0f, 1.0f,
			 -1.0f, -1.0f, 0.0f, 0.8f, 0.0f, 0.8f, 1.0f, 0.0f, 0.0f
		};
	private:
		Transform m_Transform;
		Texture m_Texture;
		unsigned int m_QuadVbo;

		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	public:
		Sprite();
		~Sprite();

		bool Init();

		unsigned int LoadTexture();

		void SetAnchor(glm::vec2 anchor);
		void SetPosition(glm::vec2 pos);
		void SetScale(glm::vec2 scale);
		void SetRotate(glm::vec2 rot);

		glm::vec2 GetAnchor();
		glm::vec2 GetPosition();
		glm::vec2 GetScale();
		glm::vec2 GetRotate();

		void SetColor(glm::vec4 color);

		float* GetQuad();
		int GetSize();

		Texture GetTexture();
		Transform &GetTransform();

	private:

	};
}


#endif // !SPRITE_H