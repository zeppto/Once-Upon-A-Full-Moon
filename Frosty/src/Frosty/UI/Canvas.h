#ifndef CANVAS_H
#define CANVAS_H
#include "UIElement.h"
#include "Frosty/UI/Sprite.h"

namespace Frosty
{

	class Canvas
	{
	public:
		float m_Vertices[9 * 6] =
		{
			-1.0f, -1.0f, 0.0f, 0.8f, 0.0f, 0.8f, 1.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f, 1.0f, 0.0f,
			 1.0f, 1.0f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f, 1.0f, 1.0f,
			 1.0f, 1.0f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f, 1.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f, 0.0f, 1.0f,
			 -1.0f, -1.0f, 0.0f, 0.8f, 0.0f, 0.8f, 1.0f, 0.0f, 0.0f
		};
	private:
		Texture m_Texture;
		Transform m_Transform;								 //Texture texture;

		std::vector<Canvas> m_Canvas;
		std::vector<UIElement> m_Elements; //Button, Text, Texture, Icon, Sprite
		std::vector<Sprite> m_Sprite;

		//include anchor in Transform class
		//glm::vec2 anchor;
		//glm::vec2 pos;
		//glm::vec2 size;


	public:
		Canvas(glm::vec2 size = glm::vec2(1, 1));
		~Canvas();

		bool Canvas::Init();
		unsigned int Canvas::LoadTexture();

		void SetAnchor(glm::vec2 anchor);
		void SetPosition(glm::vec2 pos);
		void SetScale(glm::vec2 scale);
		void SetRotate(glm::vec2 rot);

		glm::vec2 GetAnchor();
		glm::vec2 GetPosition();
		glm::vec2 GetScale();
		glm::vec2 GetRotate();

		uint16_t GetTexture();
		Transform& GetTransform();

		float* GetQuad();

	private:

	};
}


#endif // !CANVAS_H