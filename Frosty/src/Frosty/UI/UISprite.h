#ifndef UISPRITE_H
#define UISPRITE_H

#include "Frosty/UI/UIElement.h"
#include "Frosty/RenderEngine/Renderer.hpp"
#include "Frosty/RenderEngine/Texture.hpp"
#include"Frosty/API/AssetManager/AssetManager.hpp"
namespace Frosty
{
	class UISprite : public UIElement
	{
	private:
		TYPE type{ UIElement::TYPE::SPRITE };

		glm::mat4 m_Transform;
		glm::vec2 m_Translate;
		glm::vec2 m_Scale;
		glm::vec2 m_Rotate;
		glm::vec4 m_Color;

		std::string m_Image;

		uint32_t m_TextureID;
		std::shared_ptr<VertexArray> m_VertArray;

	public:
		UISprite(glm::vec2 pos = glm::vec2(0.0f), glm::vec2 scale = glm::vec2(1.0f), std::string image = "red", glm::vec4 color = glm::vec4(0.5f));
		UISprite(const UISprite& p);
		~UISprite();

		UISprite& operator= (const UISprite& other);

		inline std::shared_ptr<VertexArray>& GetVertexArray() { return m_VertArray; }
		inline glm::mat4& GetTransformSprite() 
		{


			return m_Transform; 
		}
		inline glm::vec2& GetTranslateSprite() { return m_Translate; }
		inline glm::vec2& GetScaleSprite() { return m_Scale; }
		inline glm::vec4& GetColor() { return m_Color; }

		inline uint32_t GetTextureID() { return m_TextureID; };

		inline void SetTransformSprite(glm::mat4 transform) { m_Transform = transform; }
		inline void SetTranslateSprite(glm::vec2 translate) { m_Translate = translate; m_Transform = glm::translate(glm::mat4(1.0f), glm::vec3(translate, 0.0f)); }
		inline void SetScaleSprite(glm::vec2 scale) { m_Scale = scale; m_Transform = glm::scale(m_Transform, glm::vec3(scale.x * 50, scale.y * 50, 1.0f)); }
		inline void SetRotateSprite(glm::vec2 rotate) { m_Rotate = rotate; m_Transform = glm::rotate(m_Transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); }
		inline void SetColorSprite(glm::vec4 color) { m_Color = color; }
		inline void SetImage(std::string image) { m_Image = image; m_TextureID = Frosty::AssetManager::GetTexture2D(image)->GetRenderID(); }

		TYPE GetType() { return type; };
	};
}


#endif // !UISPRITE_H