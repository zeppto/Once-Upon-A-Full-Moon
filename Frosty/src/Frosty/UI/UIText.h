#ifndef UITEXT_H
#define UITEXT_H

#include "ft2build.h"
#include FT_FREETYPE_H

#include "FreeType/freetype.h"

#include "UIElement.h"
//	copy freetype.dll from "freetype-windows-binaries-master\freetype-windows-binaries-master\win64"
//	
//	and paste in "SS - Project - master\bin\Debug - x86_64\Sandbox"

namespace Frosty
{
	class UIText : public UIElement
	{
	private:

	private:
		std::string m_text;
		std::string m_font;
		glm::vec2 m_pos;
		uint32_t m_fontSize;
		FT_Face m_face;

	public:
		UIText(std::string text = "test", std::string font = "Gabriola.ttf", uint32_t fontSize = 32);
		~UIText();

		bool LoadFont();

	public:

	};
}
#endif //!UITEXT_H