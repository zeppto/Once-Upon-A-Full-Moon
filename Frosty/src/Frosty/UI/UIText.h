#ifndef UITEXT_H
#define UITEXT_H

#include "ft2build.h"
#include FT_FREETYPE_H

#include "FreeType/freetype.h"

#include "UIElement.h"
#include "Frosty/RenderEngine/Renderer.hpp"
#include "Frosty/RenderEngine/Texture.hpp"



namespace Frosty
{
	class Application;

	class UIText : public UIElement
	{
	public:

	private:
		std::string m_Text;
		std::string m_Font;
		uint16_t m_FontSize;
		glm::vec2 m_Position;
		glm::vec3 m_Color;
		float m_FontScale; //Since we can't change the actual font size after it's been loaded without reloading it

		std::shared_ptr<VertexArray> m_VertArray;

	public:
		UIText(glm::vec2 position = glm::vec2(0.0f), std::string text = "test", glm::vec3 color = glm::vec3(0.0f), float fontScale = 1.0f, std::string font = "Gabriola", uint16_t fontSize = 32);
		~UIText();
		UIText(const UIText& that);

		const std::shared_ptr<VertexArray>& GetVertexArray() { return m_VertArray; }
		std::string GetText() const { return m_Text; }
		const glm::vec2& GetPosition();
		const glm::vec3& GetColor() { return m_Color; }
		const float GetFontScale();

		void SetText(std::string text) { m_Text = text; }
		void SetFont(std::string font) { m_Font = font; }
		void SetPosition(glm::vec2 position) { m_Position = position; }
		void SetColor(glm::vec3 color) { m_Color = color; }
		void SetFontScale(float scale) { m_FontScale = scale; }

	private:

	};
}
#endif
