#include "fypch.hpp"
#include "Assetmanager.hpp"
#include"..\PrefabManager\PrefabManager.h"
#include "..\..\DEFINITIONS.hpp"
#include"Glad/glad.h"

namespace Frosty
{
	Assetmanager* Assetmanager::s_Instance = nullptr;
	uint16_t Assetmanager::s_Total_Nr_Assets = 0;

	Assetmanager* Frosty::Assetmanager::GetAssetmanager()
	{
		if (!s_Instance)


		{
<<<<<<< HEAD
			s_Instance = FY_NEW Assetmanager;
			s_Instance->m_Total_Nr_Assets = 0;
=======
			s_Instance = new Assetmanager;
			//s_Instance->m_Total_Nr_Assets = 0;
>>>>>>> master
		}
			return s_Instance;
	}

	Frosty::Assetmanager::~Assetmanager()
	{
		//if (s_Instance != nullptr)
		//{
		//delete s_Instance;
		//}
		MotherLoader::Delete();
		PrefabManager::Delete();


	}

	bool Assetmanager::LoadFile(const std::string& FilePath, const std::string& PreFab_Name)
	{
		return MotherLoader::GetMotherLoader()->Loadfile(FilePath, PreFab_Name);
	}

	bool Assetmanager::AddNewModelTemplate(std::shared_ptr<ModelTemplate>& ModelTemplate, const FileMetaData& MetaData)
	{
		bool returnValue = false;

		if (!ModelTemplateLoaded(MetaData.FileName))
		{
			returnValue = true;

			m_ModelTemplate_File_Name_Vector.emplace_back(MetaData.FileName);

			AssetMetaData<Frosty::ModelTemplate>* tempMetaData = &m_MT_MetaData_Map[MetaData.FileName];

			tempMetaData->SetFileMetaData(MetaData);
			//tempMetaData->SetFileName(FileName);
			//tempMetaData->SetFilePath(FilePath);
			//tempMetaData->SetContainerSlot(m_AssetHolder.GetEmptyContainerSlot());
			tempMetaData->SetRefData(Frosty::ModelTemplate());

			//Material = m_AssetHolder.GetMaterial(tempMetaData->GetAssetContainerSlot());
			ModelTemplate = tempMetaData->GetData();


			//tempMetaData->SetAllRefData(*m_AssetHolder.GetModeltemplate(tempMetaData->GetAssetId()), FileName, FilePath);

			//ModelTemplate = m_AssetHolder.GetModeltemplate(tempMetaData->GetAssetId());

		}
		else
		{
			FY_CORE_WARN("Found Already Existing ModelTemplate with Asset Name: {0}", MetaData.FileName);
		}
		return returnValue;
	}

	bool Assetmanager::AddNewTextureTemplate(const TextureFile& Texture, const FileMetaData& MetaData)
	{

		bool returnValue = false;


		if (!TextureLoaded(MetaData.FileName))
		{

			m_Texture_File_Name_Vector.emplace_back(MetaData.FileName);

			AssetMetaData<TextureFile>* tempTextureMetaData = &m_Texture_MetaData_Map[MetaData.FileName];
			tempTextureMetaData->SetFileMetaData(MetaData);
			tempTextureMetaData->SetRefData(Texture);

			if (tempTextureMetaData->GetData()->LoadToGpu())
			{
				returnValue = true;
			}

		}
		else
		{
			FY_CORE_WARN("Texture are already loaded, FIleName: {0}", MetaData.FileName);
		}

		return returnValue;




		return returnValue;
	}

	bool Assetmanager::AddNewMaterialTemplate(const Luna::Material& Material, const FileMetaData& MetaData)
	{
		
		bool returnValue = false;

		if (!MaterialLoaded(MetaData.FileName))
		{
			returnValue = true;

			m_Material_File_Name_Vector.emplace_back(MetaData.FileName);

			AssetMetaData<LinkedMaterial>* tempMetaData = &m_MAT_MetaData_Map[MetaData.FileName];

			tempMetaData->SetFileMetaData(MetaData);
			tempMetaData->SetRefData(LinkedMaterial(Material));

			if (!ConnectMaterialWithTexture(*tempMetaData->GetData(), MetaData))
			{
				FY_CORE_WARN("One Or More Textures coul not connect with Material FileName: {0}", MetaData.FileName);
			}

			//Material = m_AssetHolder.GetMaterial(tempMetaData->GetAssetContainerSlot());
			//Material = tempMetaData->GetData();
		}
		else
		{
			FY_CORE_WARN("Found Already Existing Material with File Name: {0}", MetaData.FileName);
		}
		return returnValue;
	}

	std::shared_ptr<TextureFile> Assetmanager::AddNewTextureTemplate(const FileMetaData& MetaData)
	{
		std::shared_ptr<TextureFile> returnValue = nullptr;

		if (!TextureLoaded(MetaData.FileName))
		{

			//this can be better
			m_Texture_File_Name_Vector.emplace_back(MetaData.FileName);

			AssetMetaData<TextureFile>* tempTextureMetaData = &m_Texture_MetaData_Map[MetaData.FileName];
			tempTextureMetaData->SetFileMetaData(MetaData);
			tempTextureMetaData->SetRefData(TextureFile(MetaData.FullFilePath));

			if (tempTextureMetaData->GetData()->LoadToGpu())
			{
				returnValue = tempTextureMetaData->GetData();
			}

		}
		else
		{
			FY_CORE_WARN("Texture are already loaded, FIleName: {0}", MetaData.FileName);
		}

		return returnValue;


	}

	AssetMetaData<ModelTemplate> * Assetmanager::GetModeltemplateMetaData(const std::string& FileName)
	{
		if (ModelTemplateLoaded(FileName))
		{
			return &m_MT_MetaData_Map[FileName];
		}
		else
		{
			FY_CORE_WARN("Tried to fetch a non loaded ModelTemplate with AssetName: {0}", FileName);
		}
		return nullptr;
	}

	AssetMetaData<LinkedMaterial>* Assetmanager::GetMaterialMetaData(const std::string& FileName)
	{
		if (MaterialLoaded(FileName))
		{
			return &m_MAT_MetaData_Map[FileName];
		}
		else
		{
			FY_CORE_WARN("Tried to fetch a non loaded material with AssetName: {0}", FileName);
		}
		return nullptr;
	}

	AssetMetaData<TextureFile>* Assetmanager::GetTextureMetaData(const std::string& FileName)
	{
		if (TextureLoaded(FileName))
		{
			return &m_Texture_MetaData_Map[FileName];
		}
		else
		{
			FY_CORE_WARN("Tried to fetch a non loaded texture with AssetName: {0}", FileName);
		}
		return nullptr;
	}

	bool Assetmanager::LinkModelKey(const std::string& FileName, KeyLabel<ModelTemplate>* In_Key)
	{

		bool returnValue = false;

		if (ModelTemplateLoaded(FileName)) {
			In_Key->SetKeyData(m_MT_MetaData_Map[FileName]);
			returnValue = true;
		}
		else
		{
			FY_CORE_WARN("Could not link Modeltemplate key, File is not loaded, File: {0}", FileName);
		}

		return returnValue;
	}

	bool Assetmanager::LinkMaterialKey(const std::string& FileName, KeyLabel<LinkedMaterial>* In_Key)
	{
		bool returnValue = false;

		if (ModelTemplateLoaded(FileName)) {
			In_Key->SetKeyData(m_MAT_MetaData_Map[FileName]);
			returnValue = true;
		}
		else
		{
			FY_CORE_WARN("Could not link Material key, File is not loaded, File: {0}",FileName);
		}
		return returnValue;
	}

	bool Assetmanager::LinkKey(const std::string& FileName, BaseKey* In_Key)
	{
		bool returnValue = false;

		if (ModelTemplateLoaded(FileName)) {

			KeyLabel<ModelTemplate>* temp_mt_ptr = static_cast<KeyLabel<ModelTemplate>*>(In_Key);
			temp_mt_ptr->SetKeyData(m_MT_MetaData_Map[FileName]);
			returnValue = true;
		}
		else if (MaterialLoaded(FileName))
		{
			KeyLabel<LinkedMaterial>* temp_mt_ptr = static_cast<KeyLabel<LinkedMaterial>*>(In_Key);
			temp_mt_ptr->SetKeyData(m_MAT_MetaData_Map[FileName]);
			returnValue = true;
		}
		else 
		{
			FY_CORE_WARN("Could not link key, The FileName is not loaded, Name: {0}",FileName);
		}
		return returnValue;
	}

	bool Assetmanager::ConnectMaterialWithTexture(LinkedMaterial& Material, const FileMetaData& MetaData)
	{
		bool returnValue = true;
		bool GlowLoadSuccess = true;
		bool NormalLoadSuccess = true;
		bool DiffuseLoadSuccess = true;

		//Diffuse Linking
		Material.Diffuse_File_Name = CutFileExtentionFromString(Material.LunaMaterial.diffuseTexPath);
		if (Material.Diffuse_File_Name != "")
		{
			if (!TextureLoaded(Material.Diffuse_File_Name))
			{
				//can be shorted
				DiffuseLoadSuccess = MotherLoader::GetMotherLoader()->Loadfile((PROJECT_LUNAFILES_FOLDER_ROOT + std::string(Material.LunaMaterial.diffuseTexPath)));
				if (DiffuseLoadSuccess)
				{
					Material.Diffuse_Texture_MetaData_Ptr = GetTextureMetaData(Material.Diffuse_File_Name);
				}
				else
				{
					FY_CORE_WARN("Diffuse Material Name: {0} Could not be loaded, File Path: {1}" , Material.Diffuse_File_Name, MetaData.FilePath);
				}

			}
			else
			{
				Material.Diffuse_Texture_MetaData_Ptr = GetTextureMetaData(Material.Diffuse_File_Name);
			}
		}
		else
		{
			FY_CORE_WARN("Diffuse Material has no name, File Path: {0}", MetaData.FilePath);
		}


		if (Material.LunaMaterial.hasNormalMap)
		{
			//Normal Linking
			Material.Normal_File_Name = CutFileExtentionFromString(Material.LunaMaterial.normalTexPath);
			if (Material.Normal_File_Name != "")
			{
				if (!TextureLoaded(Material.Normal_File_Name))
				{
					NormalLoadSuccess = MotherLoader::GetMotherLoader()->Loadfile((PROJECT_LUNAFILES_FOLDER_ROOT + std::string(Material.LunaMaterial.normalTexPath)));
					if (NormalLoadSuccess)
					{
						Material.Normal_Texture_MetaData_Ptr = GetTextureMetaData(Material.Normal_File_Name);
					}
					else
					{
						FY_CORE_WARN("Normal Material Name: {0} Could not be loaded, File Path: {1}", Material.Normal_File_Name, MetaData.FilePath);
					}
				}
				else
				{
					Material.Normal_Texture_MetaData_Ptr = GetTextureMetaData(Material.Normal_File_Name);
				}
			}
			else
			{
				FY_CORE_WARN("Normal Material has no name, File Path: {0}", MetaData.FilePath);
			}

		}

		if (Material.LunaMaterial.hasGlowMap)
		{

			//Glow Linking
			Material.Glow_File_Name = CutFileExtentionFromString(Material.LunaMaterial.glowTexPath);
			if (Material.Glow_File_Name != "")
			{
				if (!TextureLoaded(Material.Glow_File_Name))
				{
					GlowLoadSuccess = MotherLoader::GetMotherLoader()->Loadfile((PROJECT_LUNAFILES_FOLDER_ROOT + std::string(Material.LunaMaterial.glowTexPath)));
					if (GlowLoadSuccess)
					{
						Material.Glow_Texture_MetaData_Ptr = GetTextureMetaData(Material.Glow_File_Name);
					}
					else
					{
						FY_CORE_WARN("Glow Material Name: {0} Could not be loaded, File Path: {1}", Material.Glow_File_Name, MetaData.FilePath);
					}

				}
				else
				{
					Material.Glow_Texture_MetaData_Ptr = GetTextureMetaData(Material.Glow_File_Name);
				}
			}
			else
			{
				FY_CORE_WARN("Glow Material has no name, File Path: {0}", MetaData.FilePath);
			}

		}

		if (!(GlowLoadSuccess && NormalLoadSuccess && DiffuseLoadSuccess))
		{
			returnValue = false;
		}


		return returnValue;
	}

	bool Assetmanager::FileLoaded(const std::string& FilePath)
	{
		bool returnValue = false;

		for (int i = 0; i < m_FilePath_Vector.size() && returnValue == false; i++)
		{
			if (m_FilePath_Vector.at(i) == FilePath)
			{
				returnValue = true;
			}
		}
		return returnValue;
	}

	bool Assetmanager::AssetLoaded(const std::string& FileName)
	{
		bool returnValue = false;

		for (int i = 0; i < m_Material_File_Name_Vector.size() && returnValue == false; i++)
		{
			if (m_Material_File_Name_Vector.at(i) == FileName)
			{
				returnValue = true;
			}
		}

		for (int i = 0; i < m_ModelTemplate_File_Name_Vector.size() && returnValue == false; i++)
		{
			if (m_ModelTemplate_File_Name_Vector.at(i) == FileName)
			{
				returnValue = true;
			}
		}

		return returnValue;
	}

	bool Assetmanager::MaterialLoaded(const std::string& FileName)
	{
		bool returnValue = false;

		for (int i = 0; i < m_Material_File_Name_Vector.size() && returnValue == false; i++)
		{
			if (m_Material_File_Name_Vector.at(i) == FileName)
			{
				returnValue = true;
			}
		}
		return returnValue;
	}

	bool Assetmanager::ModelTemplateLoaded(const std::string& FileName)
	{
		bool returnValue = false;
		for (int i = 0; i < m_ModelTemplate_File_Name_Vector.size() && returnValue == false; i++)
		{
			if (m_ModelTemplate_File_Name_Vector.at(i) == FileName)
			{
				returnValue = true;
			}
		}

		return returnValue;
	}

	bool Assetmanager::TextureLoaded(const std::string& FileName)
	{
		bool returnValue = false;
		for (int i = 0; i < m_Texture_File_Name_Vector.size() && returnValue == false; i++)
		{
			if (m_Texture_File_Name_Vector.at(i) == FileName)
			{
				returnValue = true;
			}
		}

		return returnValue;
	}

	const std::string Assetmanager::CutFileExtentionFromString(const char* in_char_ptr)
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

	//const std::string Assetmanager::CharToTring(const char* in_char_ptr)
	//{

	//	uint16_t count = 0;

	//	std::string aString(in_char_ptr);

	//	std::string returnString = "";
	//	while (in_char_ptr[0] != '\0')
	//	{
	//		returnString += in_char_ptr[count++];
	//	}

	//	return returnString;
	//}

}