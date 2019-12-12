#include "fypch.hpp"
#include "TextureFile.hpp"
#include"stb_image/stb_image.h"
#include"Glad/glad.h"

namespace Frosty
{


	TextureFile::TextureFile(const FileMetaData& MetaData) : AssetFile(MetaData)
	{
		m_Components = -1;
		m_Image_Width = 0;
		m_Image_Height = 0;
		m_ImageData = nullptr;
	}

	TextureFile::~TextureFile()
	{
		if (m_OnGPU)
		{
			DeleteFromGPU();
		}
		if (m_OnMem)
		{
			DeleteFromMem();
		}
	}

	bool TextureFile::LoadToMem(const bool& Reload)
	{

		if (!m_OnMem || Reload)
		{
			if (m_OnMem)
			{
				DeleteFromMem();
			}
			m_ImageData = stbi_load((m_MetaData.FullFilePath.c_str()), &m_Image_Width, &m_Image_Height, &m_Components, STBI_rgb_alpha);

			if (m_ImageData != nullptr)
			{
				m_OnMem = true;
			}
			else
			{
				FY_CORE_WARN("STBI Could not load image, FilePath: {0}", m_MetaData.FullFilePath);
			}
		}
		else
		{
			FY_CORE_INFO("Texture: {0}, Is already loaded on Memory", m_MetaData.FileName);
		}

		return !m_OnMem;
	}

	bool TextureFile::DeleteFromMem()
	{
		if (m_OnMem)
		{
			if (m_ImageData != nullptr)
			{
				stbi_image_free(m_ImageData);
				m_OnMem = false;
			}
			else
			{
				FY_CORE_WARN("Texture: {0},  Ptr was Null in a delete", m_MetaData.FileName);
			}
		}
		{
			FY_CORE_INFO("Texture: {0},  Is already deleted from Memory", m_MetaData.FileName);
		}
		return !m_OnMem;
	}

	bool TextureFile::LoadToGPU()
	{
		bool returnValue = false;

		if (m_ImageData != nullptr)
		{

			if (m_OnGPU)
			{
				DeleteFromGPU();
			}


			returnValue = true;
			glGenTextures(1, &m_GPU_Data_ID);
		//	glActiveTexture(m_GPU_Data_ID);
			glBindTexture(GL_TEXTURE_2D, m_GPU_Data_ID);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, m_Image_Width, m_Image_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_ImageData);
			glGenerateMipmap(GL_TEXTURE_2D);

			m_OnGPU = true;

			glBindTexture(GL_TEXTURE_2D, 0);

		}
		else
		{
			FY_CORE_WARN("Cannot load a empty file into the GPU, FilePath: {0}", m_MetaData.FullFilePath);
		}

		return returnValue;

	}

	bool TextureFile::DeleteFromGPU()
	{

		glDeleteTextures(1, &m_GPU_Data_ID);

		m_OnGPU = false;
		m_GPU_Data_ID = -1;

		m_Image_Width = 0;
		m_Image_Height = 0;

		//Open gl Error check?

		return true;
	}


}