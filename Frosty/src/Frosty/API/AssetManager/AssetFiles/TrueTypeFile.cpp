#include "fypch.hpp"
#include "TrueTypeFile.hpp"

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

		if (FT_Load_Char(m_face, 'X', FT_LOAD_RENDER))
		{
			FY_CORE_ERROR("ERROR::FREETYPE: Failed to load glyph.");
		}
		else
		{
			FY_CORE_INFO("INFO::FREETYPE: Glyph successfully loaded.");
		}

		FT_Done_Face(m_face);
		FT_Done_FreeType(freetype);

		return true;
	}
}