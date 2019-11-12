#include "fypch.hpp"
#include "UIText.h"
#include "Frosty/DEFINITIONS.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"


namespace Frosty
{
	UIText::UIText(std::string text, std::string font, uint16_t fontSize)
	{
		m_type = TEXT;
		m_Text = text;
		m_Font = font;
		m_FontSize = fontSize;

		std::string::const_iterator c;

		float x = 100.0f;
		float y = 24.0f;
		float scale = 1.0f;
		glm::vec3 color(0.0, 0.0, 1.0);

		for (c = m_Text.begin(); c != m_Text.end(); c++)
		{
			Frosty::Character ch = AssetManager::GetTTF(font)->m_characters.at(*c);

			float xPos = x + ch.bearing.x * scale;
			float yPos = y - (ch.size.y - ch.bearing.y) * scale;
			float zPos = 0.0f;
			float width = ch.size.x * scale;
			float height = ch.size.y * scale;

			//Create quad

			//Pos_3, Uv_2, Color_4
			float vertices[(3 + 2 + 4) * 6]{
				xPos,		  yPos + height, zPos,	0.0f, 0.0f, color.r, color.b, color.b,
				xPos,		  yPos,			 zPos,	0.0f, 1.0f, color.r, color.b, color.b,
				xPos + width, yPos,			 zPos,	1.0f, 1.0f, color.r, color.b, color.b,

				xPos,		  yPos + height, zPos,	0.0f, 0.0f, color.r, color.b, color.b,
				xPos + width, yPos,			 zPos,	1.0f, 1.0f, color.r, color.b, color.b,
				xPos + width, yPos + height, zPos,	1.0f, 0.0f, color.r, color.b, color.b
			};

			////Send quad to Assetmanager()
			////	vertices[]
			////Send texture to Assetmanager()
			////	ch.textureID
			//
			//const int32_t quadID = 1; // = Assetmanager::GetAssetmanager()->GetModeltemplateMetaData("quad")->GetAssetId();

			////Get pointer

			////This array should have pointers (from Assetmanager())
			//m_quadPtrArray.emplace_back(quadID);


			x += (ch.advance >> 6) * scale;
		}

	}

	UIText::~UIText()
	{

	}


	bool UIText::Init()
	{

		//float x = 100.0f;
		//float y = 24.0f;
		//float scale = 1.0f;
		//glm::vec3 vec = glm::vec3(0.5f, 0.8f, 0.2f);

		//for (std::string::const_iterator c = m_text.begin(); c != m_text.end(); c++)
		//{
		//	Character ch = Assetmanager::GetAssetmanager()->GetFontMetaData(m_font)->GetData()->m_characters.at(*c);
		//	float xpos = x + ch.bearing.x * scale;
		//	float ypos = y - (ch.size.y - ch.bearing.y) * scale;
		//	float width = ch.size.x * scale;
		//	float height = ch.size.y * scale;

		//	float verts[6][4]
		//	{
		//		{ xpos,			ypos + height,	0.0f, 0.0f },
		//		{ xpos,			ypos,			0.0f, 1.0f },
		//		{ xpos + width,	ypos,			1.0f, 1.0f },

		//		{ xpos,			ypos + height,	0.0f, 0.0f },
		//		{ xpos + width, ypos,			1.0f, 1.0f },
		//		{ xpos + width, ypos + height,	1.0f, 0.0f }
		//	};

		//}


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