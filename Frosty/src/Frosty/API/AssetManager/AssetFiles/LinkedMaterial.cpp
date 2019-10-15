#include"fypch.hpp"
#include"AssetFile.hpp"
#include"LinkedMaterial.hpp"
#include "Luna/include/Luna.h"
#include "TextureFile.hpp"
#include"..\AM.hpp"

namespace Frosty
{

	LinkedMaterial::~LinkedMaterial()
	{
		m_Glow_Texture_Ptr = nullptr;
		m_Normal_Texture_Ptr = nullptr;
		m_Diffuse_Texture_Ptr = nullptr;
	}

	void LinkedMaterial::LinkMaterial()
	{
		m_Glow_Texture_Ptr = Assetmanager::GetTexture(CutFileExtentionFromString(m_LunaMaterial.diffuseTexPath));
		m_Normal_Texture_Ptr = Assetmanager::GetTexture(CutFileExtentionFromString(m_LunaMaterial.diffuseTexPath));
		m_Diffuse_Texture_Ptr = Assetmanager::GetTexture(CutFileExtentionFromString(m_LunaMaterial.diffuseTexPath));
	}

	bool LinkedMaterial::LoadToMem(const bool& Reload)
	{
		//Continue from here with loader
		FY_CORE_INFO("Linked Material Cannot Load To Mem");
		return false;
	}

	bool LinkedMaterial::DeleteFromMem()
	{
		FY_CORE_INFO("Linked Material Cannot Delete From Mem");
		return false;
	}

	bool LinkedMaterial::LoadToGPU()
	{
		FY_CORE_INFO("Linked Material Cannot Load To GPU");
		return false;
	}

	bool LinkedMaterial::DeleteFromGPU()
	{
		FY_CORE_INFO("Linked Material Cannot Load From GPU");
		return false;
	}

	const std::string LinkedMaterial::CutFileExtentionFromString(const char* in_char_ptr)
	{
		std::string returnString = "";


		if (*in_char_ptr != '\0')
		{

			uint8_t count = 0;
			while (in_char_ptr[count] != '\0')
			{
				if (in_char_ptr[count] != '.')
				{
					returnString.push_back(in_char_ptr[count]);
				}
				else
				{
					break;
				}
				count++;
			}
		}
		else
		{
			FY_CORE_WARN("Trying to cut a file extention from a emtpy char ptr");
		}
		return returnString;
	}







}