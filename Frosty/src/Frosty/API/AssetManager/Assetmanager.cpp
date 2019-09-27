#include "fypch.hpp"
#include "Assetmanager.hpp"
#include"..\PrefabManager\PrefabManager.h"
#include "..\..\DEFINITIONS.hpp"


namespace Frosty
{
	Assetmanager* Assetmanager::s_Instance = nullptr;

	Assetmanager* Frosty::Assetmanager::GetAssetmanager()
	{
		if (!s_Instance)


		{
			s_Instance = new Assetmanager;
			s_Instance->m_Total_Nr_Assets = 0;
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

		if (!ModelTemplateLoaded(FileName))
		{
			returnValue = true;

			m_ModelTemplate_File_Name_Vector.emplace_back(FileName);

			AssetMetaData<Frosty::ModelTemplate>* tempMetaData = &m_MT_MetaData_Map[FileName];

			tempMetaData->SetFileName(FileName);
			tempMetaData->SetFilePath(FilePath);
			tempMetaData->SetContainerSlot(m_AssetHolder.GetEmptyContainerSlot());
			tempMetaData->SetRefData(Frosty::ModelTemplate());

			//Material = m_AssetHolder.GetMaterial(tempMetaData->GetAssetContainerSlot());
			ModelTemplate = tempMetaData->GetData();


			//tempMetaData->SetAllRefData(*m_AssetHolder.GetModeltemplate(tempMetaData->GetAssetId()), FileName, FilePath);

			//ModelTemplate = m_AssetHolder.GetModeltemplate(tempMetaData->GetAssetId());

		}
		else
		{
			FY_CORE_WARN("Found Already Existing ModelTemplate with Asset Name: {0}", FileName);
		}
		return returnValue;
	}

	bool Assetmanager::AddNewTextureTemplate(std::shared_ptr<TextureFile>& Texture, const FileMetaData& MetaData)
	{
		//START HERE
		return false;
	}

	bool Assetmanager::AddNewMaterialTemplate(const Luna::Material& Material, const FileMetaData& MetaData)
	{
		
		bool returnValue = false;

		if (!MaterialLoaded(MetaData.FileName))
		{
			returnValue = true;

			m_Material_File_Name_Vector.emplace_back(MetaData.FileName);

			AssetMetaData<Luna::Material>* tempMetaData = &m_MAT_MetaData_Map[MetaData.FileName];

			tempMetaData->SetFileMetaData(MetaData);
			//tempMetaData->SetFilePath(MetaData.FullFilePath);
			//tempMetaData->SetContainerSlot(m_AssetHolder.GetEmptyContainerSlot());
			tempMetaData->SetRefData(Material);

			if (!ConnectMaterialWithTexture(Material))
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

	AssetMetaData<Luna::Material>* Assetmanager::GetMaterialMetaData(const std::string& FileName)
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

	bool Assetmanager::LinkMaterialKey(const std::string& FileName, KeyLabel<Luna::Material>* In_Key)
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
			KeyLabel<Luna::Material>* temp_mt_ptr = static_cast<KeyLabel<Luna::Material>*>(In_Key);
			temp_mt_ptr->SetKeyData(m_MAT_MetaData_Map[FileName]);
			returnValue = true;
		}
		else 
		{
			FY_CORE_WARN("Could not link key, The FileName is not loaded, Name: {0}",FileName);
		}
		return returnValue;
	}

	bool Assetmanager::ConnectMaterialWithTexture(const Luna::Material& Material, const FileMetaData& MetaData)
	{


	//	if(Material.hasNormalMap)

		//Check if loaded

		bool returnValue = true;
		// this needs to be saved MARK for later
		std::string temp_fileName = CutFileExtentionFromString(Material.diffuseTexPath);

		if(!TextureLoaded(temp_fileName))
		{
			returnValue = MotherLoader::GetMotherLoader()->Loadfile((PROJECT_LUNAFILES_FOLDER_ROOT + *Material.diffuseTexPath));
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
		for (uint16_t i = 0; i < sizeof(*in_char_ptr); i++)
		{
			if (in_char_ptr[i] != '.')
			{
				returnString.push_back(in_char_ptr[i]);
			}
			else
			{
				break;
			}
		}
		return returnString;
	}

}