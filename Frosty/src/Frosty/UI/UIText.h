#ifndef UITEXT_H
#define UITEXT_H

#include "ft2build.h"
#include FT_FREETYPE_H

#include "FreeType/freetype.h"

#include "UIElement.h"

namespace Frosty
{
	class UIText : public UIElement
	{
		struct Character
		{
			uint32_t textureID; //ID-Handle of the glyph texture
			glm::ivec2 size;	//Glyph-size
			glm::ivec2 bearing;	//Offset from baselne to left/top of glyph
			uint32_t advance;	//Offset to advance to next glyph
		};

	private:

	private:
		std::string m_text;
		std::string m_font;
		glm::vec2 m_pos;
		uint32_t m_fontSize;

		FT_Face m_face;

		std::map<char, Character> m_characters;

	public:
		UIText(std::string text = "test", std::string font = "Gabriola.ttf", uint32_t fontSize = 32);
		~UIText();

		bool LoadFont();

	public:

	};
}
#endif //!UITEXT_H