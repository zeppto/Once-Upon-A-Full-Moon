#include "fypch.hpp"
#include "Assetmanager.hpp"
#include"..\PrefabManager\PrefabManager.h"


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

	bool Assetmanager::LoadFile(const std::string& FilePath, const std::string& PreFab_Name)
	{
		return MotherLoader::GetMotherLoader()->Loadfile(FilePath, PreFab_Name);


		//if (1) //check if asset already exist
		//{
		//	auto Loader = MotherLoader::GetMotherLoader();
		//	std::vector<Luna::Material> tempMaterialVector;

		//	Loader->Loadfile(
		//		FilePath,
		//		*m_AssetHolder.GetModeltemplate(0),
		//		tempMaterialVector
		//		);



		//	if (1)// check if material exists
		//	{

		//		// Temp Function for testing
		//		for (int i = 0; i < tempMaterialVector.size(); i++)
		//		{
		//			*m_AssetHolder.GetMaterial(m_NrOfMaterial++) = tempMaterialVector.at(i);
		//		}
		//	}


	//	}


	//	return true;
	}

	//bool Assetmanager::LoadFile(std::string FilePath, const std::string& set_Prefabkey)
	//{

	//	bool returnValue = false;

	//	if (MotherLoader::GetMotherLoader()->Loadfile(FilePath, set_Prefabkey))
	//	{
	//		FY_CORE_INFO("SUCCESS LOADING FILE: {0}" , FilePath);
	//		returnValue = true;
	//	}
	//	else
	//	{
	//		FY_CORE_WARN("FAILED TO LOAD FILE: {0}", FilePath);
	//	}


	//	return returnValue;
	//}

	bool Assetmanager::AddNewModelTemplate(std::shared_ptr<ModelTemplate>& ModelTemplate, const std::string& FileName, const std::string& FilePath)
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

	bool Assetmanager::AddNewMaterialTemplate(std::shared_ptr<Luna::Material>& Material, const std::string& FileName, const std::string& FilePath)
	{
		
		bool returnValue = false;

		if (!MaterialLoaded(FileName))
		{
			returnValue = true;

			m_Material_File_Name_Vector.emplace_back(FileName);

			AssetMetaData<Luna::Material>* tempMetaData = &m_MAT_MetaData_Map[FileName];

			tempMetaData->SetFileName(FileName);
			tempMetaData->SetFilePath(FilePath);
			tempMetaData->SetContainerSlot(m_AssetHolder.GetEmptyContainerSlot());
			tempMetaData->SetRefData(*m_AssetHolder.GetMaterial(tempMetaData->GetAssetContainerSlot()));

			//Material = m_AssetHolder.GetMaterial(tempMetaData->GetAssetContainerSlot());
			Material = tempMetaData->GetData();
		}
		else
		{
			FY_CORE_WARN("Found Already Existing Material with File Name: {0}", FileName);
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

	Frosty::Assetmanager::~Assetmanager()
	{
		//if (s_Instance != nullptr)
		//{
		//delete s_Instance;
		//}
		MotherLoader::Delete();
		PrefabManager::Delete();
		
	
	}
}