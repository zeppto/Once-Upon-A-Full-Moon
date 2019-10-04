#include "fypch.hpp"
#include "UIText.h"
#include "Frosty/DEFINITIONS.hpp"

namespace Frosty
{
	UIText::UIText(std::string text, std::string font, uint32_t fontSize)
	{
		m_text = text;
		m_font = font;
		m_fontSize = fontSize;
		//m_face = nullptr;
	}

	UIText::~UIText()
	{

	}

	bool UIText::LoadFont()
	{
		//FT_Library freetype;
		//if (FT_Init_FreeType(&freetype))
		//{
		//	FY_CORE_ERROR("ERROR::FREETYPE: FreeType library could not be initialized.");
		//	return false;
		//}
		//else
		//{
		//	FY_CORE_INFO("INFO::FREETYPE: FreeType successfully initialized.");
		//}

		//std::string path = FY_FONTS_FOLDER_ROOT;
		//if (FT_New_Face(freetype, ((std::string)path + m_font).c_str(), 0, &m_face))
		//{
		//	FY_CORE_ERROR("ERROR::FREETYPE: Failed to load font.");
		//	return false;
		//}
		//else 
		//{
		//	FY_CORE_INFO("INFO::FREETYPE: Font successfully loaded.");
		//}

		//FT_Set_Pixel_Sizes(m_face, 0, m_fontSize);

		//if (FT_Load_Char(m_face, 'X', FT_LOAD_RENDER))
		//{
		//	FY_CORE_ERROR("ERROR::FREETYPE: Failed to load glyph.");
		//}
		//else
		//{
		//	FY_CORE_INFO("INFO::FREETYPE: Glyph successfully loaded.");
		//}

		//FT_Done_Face(m_face);
		//FT_Done_FreeType(freetype);

		return true;
	}

}