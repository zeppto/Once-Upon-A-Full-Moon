#ifndef TRUETYPEFILE_HPP
#define TRUETYPEFILE_HPP

#include "ft2build.h"
#include FT_FREETYPE_H

#include "FreeType/freetype.h"

namespace Frosty
{
	struct Character
	{
		uint32_t textureID; //ID-Handle of the glyph texture
		glm::ivec2 size;	//Glyph-size
		glm::ivec2 bearing;	//Offset from baselne to left/top of glyph
		uint32_t advance;	//Offset to advance to next glyph
	};

	class TrueTypeFile
	{
	public:
		std::map<char, Character> m_characters;
	private:
		std::string m_filePath;
		const uint8_t FONTSIZE = 48;

	public:
		TrueTypeFile(const std::string& FilePath);
		~TrueTypeFile();

		bool LoadFont();

	private:
		
	};
}

#endif //!TRUETYPEFILE_HPP