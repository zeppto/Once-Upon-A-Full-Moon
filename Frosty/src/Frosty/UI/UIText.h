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
	class UIText : public UIElement
	{
	public:

	private:
		std::string m_Text;
		std::string m_Font;
		uint16_t m_FontSize;

		std::shared_ptr<Texture2D> m_Texture;
		std::shared_ptr<VertexArray> m_VertArray;

	public:
		UIText(std::string text = "test", std::string font = "Gabriola", uint16_t fontSize = 32);
		~UIText();

		const std::shared_ptr<VertexArray>& GetVertexArray() { return m_VertArray; };
		std::string GetText() const { return m_Text; };
		const std::shared_ptr<Texture2D>& GetTexture() { return m_Texture; };

		void SetText(std::string text) { m_Text = text; };
		void SetFont(std::string font) { m_Font = font; };

	private:

	};
}
#endif
