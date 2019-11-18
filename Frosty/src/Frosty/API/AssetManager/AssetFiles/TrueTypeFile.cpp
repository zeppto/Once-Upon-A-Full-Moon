#include "fypch.hpp"
#include "TrueTypeFile.hpp"
#include"Glad/glad.h"

namespace Frosty
{

	Frosty::TrueTypeFile::TrueTypeFile(const std::string& FilePath)
	{
		m_filePath = FilePath;
	}

	Frosty::TrueTypeFile::~TrueTypeFile()
	{

	}

	bool Frosty::TrueTypeFile::LoadFont()
	{
		FT_Library freetype;
		FT_Face m_face;
		if (FT_Init_FreeType(&freetype))
		{
			FY_CORE_ERROR("ERROR::FREETYPE: FreeType library could not be initialized.");
			return false;
		}
		else
		{
			FY_CORE_INFO("INFO::FREETYPE: FreeType successfully initialized.");
		}

		if (FT_New_Face(freetype, m_filePath.c_str(), 0, &m_face))
		{
			FY_CORE_ERROR("ERROR::FREETYPE: Failed to load font.");
			return false;
		}
		else
		{
			FY_CORE_INFO("INFO::FREETYPE: Font successfully loaded.");
		}

		FT_Set_Pixel_Sizes(m_face, 0, FONTSIZE);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
		for (GLubyte c = 0; c < 128; c++)
		{
			if (FT_Load_Char(m_face, c, FT_LOAD_RENDER)) //Load glyph
			{
				FY_CORE_ERROR("ERROR::FREETYPE: Failed to load glyph.");
				continue;
			}

			//Generate texture
			uint32_t texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, m_face->glyph->bitmap.buffer);

			//Texture settings
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			//Store the information
			Character character =
			{
				texture,
				glm::ivec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
				glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
				(uint32_t)m_face->glyph->advance.x
			};

			//uint32_t textureID; //ID-Handle of the glyph texture
			//glm::ivec2 size;	//Glyph-size
			//glm::ivec2 bearing;	//Offset from baselne to left/top of glyph
			//uint32_t advance;	//Offset to advance to next glyph

			m_characters.insert(std::pair<char, Character>(c, character));
		}

		FT_Done_Face(m_face);
		FT_Done_FreeType(freetype);

		return true;
	}
}