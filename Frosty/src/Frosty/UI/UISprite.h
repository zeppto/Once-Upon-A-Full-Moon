#ifndef UISPRITE_H
#define UISPRITE_H

#include "Frosty/UI/UIElement.h"

namespace Frosty
{
	class UISprite : public UIElement
	{
	public:


	private:
		TYPE type = SPRITE;

		float* m_vertices;
		uint32_t m_TextureID;

	public:
		UISprite();
		~UISprite();

		bool Init();

		unsigned int LoadTexture();

		float* GetVertices() { return m_vertices; };
		uint8_t GetVerticesSize() { return sizeof(*m_vertices) / sizeof(float); };

		uint32_t GetTexture() { return m_TextureID; };

		TYPE GetType() { return type; };

	private:

	};
}


#endif // !UISPRITE_H