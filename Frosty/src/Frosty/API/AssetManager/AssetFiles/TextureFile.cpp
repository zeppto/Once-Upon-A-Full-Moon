#include "fypch.hpp"
#include "TextureFile.hpp"
#include"stb_image.hpp"
#include"Glad/glad.h"

namespace Frosty
{


	TextureFile::TextureFile()
	{
		m_FilePath = "";
		m_Buffer_ID = -1;
		m_Components = -1;
		m_Image_Width = 0;
		m_Image_Height = 0;
		m_Loaded_In_Gpu = false;
	}

	TextureFile::TextureFile(const std::string& FilePath)
	{
		m_Buffer_ID = -1;
		m_Components = -1;
		m_Image_Width = 0;
		m_Image_Height = 0;
		m_FilePath = FilePath;
		m_Loaded_In_Gpu = false;
	}

	TextureFile::~TextureFile()
	{
		ReleaseFromGpu();
	}

	bool TextureFile::LoadToGpu()
	{
		bool returnValue = false;

		unsigned int tempBufferId = -1;
		int textureWidth, textureHeight, components;
		unsigned char* imageData = stbi_load((m_FilePath.c_str()), &textureWidth, &textureHeight, &components, STBI_rgb_alpha);

		if (imageData != nullptr)
		{

			if (m_Loaded_In_Gpu)
			{
				ReleaseFromGpu();
			}


			returnValue = true;
			glGenTextures(1, &tempBufferId);
			glActiveTexture(tempBufferId);
			glBindTexture(GL_TEXTURE_2D, tempBufferId);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);

			m_Buffer_ID = (uint16_t)tempBufferId;
			m_Image_Height = (uint16_t)textureHeight;
			m_Image_Width = (uint16_t)textureWidth;
			m_Components = (uint8_t)components;
			m_Loaded_In_Gpu = true;

		}
		else
		{
			FY_CORE_WARN("STBI Could not load image, FilePath: {0}", m_FilePath);
		}
		stbi_image_free(imageData);
		return returnValue;
	}

	bool TextureFile::LoadToGpu(const std::string& FilePath)
	{
		bool returnValue = false;

		unsigned int tempBufferId = (uint16_t)(-1);
		int textureWidth, textureHeight, components;
		unsigned char* imageData = stbi_load((FilePath.c_str()), &textureWidth, &textureHeight, &components, STBI_rgb_alpha);

		if (imageData != nullptr)
		{

			if (m_Loaded_In_Gpu)
			{
				ReleaseFromGpu();
			}

			returnValue = true;
			glGenTextures(1, &tempBufferId);
			glActiveTexture(tempBufferId);
			glBindTexture(GL_TEXTURE_2D, tempBufferId);
	

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);

			m_Buffer_ID = (uint16_t)tempBufferId;
			m_Image_Height = (uint16_t)textureHeight;
			m_Image_Width = (uint16_t)textureWidth;
			m_Components = (uint8_t)components;
			m_FilePath = FilePath;
			m_Loaded_In_Gpu = true;

		}
		else
		{
			FY_CORE_WARN("STBI Could not load image, FilePath: {0}", FilePath);
		}
		stbi_image_free(imageData);
		return returnValue;
	}

	void TextureFile::ReleaseFromGpu()
	{

		unsigned int i = m_Buffer_ID;
		glDeleteTextures(1, &i);

		m_Loaded_In_Gpu = false;
		m_Buffer_ID = -1;
		m_Image_Width = 0;
		m_Image_Height = 0;

	}


}