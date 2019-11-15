#ifndef UISPRITE_H
#define UISPRITE_H

#include "Frosty/UI/UIElement.h"
#include "Frosty/RenderEngine/Renderer.hpp"
#include "Frosty/RenderEngine/Texture.hpp"

namespace Frosty
{
	class UISprite : public UIElement
	{
	private:
		TYPE type{ UIElement::TYPE::SPRITE };

		float* m_vertices;
		uint32_t m_TextureID;

		std::shared_ptr<Texture2D> m_Texture;
		std::shared_ptr<VertexArray> m_VertArray;

	public:
		UISprite();
		~UISprite();

		//bool Init();

		void LoadTexture();

		float* GetVertices() { return m_vertices; };
		uint8_t GetVerticesSize() { return sizeof(*m_vertices) / sizeof(float); };

		uint32_t GetTexture() { return m_TextureID; };

		TYPE GetType() { return type; };

	};
}


#endif // !UISPRITE_H