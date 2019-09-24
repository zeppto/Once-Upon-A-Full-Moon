#include "fypch.hpp"
#include "Assetmanager.hpp"



namespace Frosty
{
	Assetmanager* Assetmanager::m_Instance = nullptr;

	Assetmanager* Frosty::Assetmanager::GetAssetmanager()
	{
		if (!m_Instance)
		{
			m_Instance = new Assetmanager;
			m_Instance->m_NrOfMaterial = 0;
		}
			return m_Instance;
	}

	bool Assetmanager::LoadFile(std::string FilePath)
	{

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

		MotherLoader::GetMotherLoader()->Loadfile(FilePath);

	//	}


		return true;
	}

	bool Assetmanager::LoadFile(std::string FilePath, const std::string& set_Prefabkey)
	{

		if (MotherLoader::GetMotherLoader()->Loadfile(FilePath, set_Prefabkey))
		{
			FY_CORE_INFO("SUCCESS LOADING FILE: {0}" , FilePath);
		}
		else
		{
			FY_CORE_INFO("FAILED TO LOAD FILE: {0}", FilePath);
		}


		return false;
	}

	bool Assetmanager::AddNewModelTemplate(ModelTemplate*& ModelTemplate, const std::string& AssetName, const std::string& FileName)
	{
		bool returnValue = false;

		if (!ModelTemplateLoaded(AssetName))
		{
			returnValue = true;

			m_ModelTemplate_Asset_Name_Vector.emplace_back(AssetName);

			AssetMetaData<Frosty::ModelTemplate>* tempMetaData = &m_MT_MetaData_Map[AssetName];

			tempMetaData->SetRefAllData(*m_AssetHolder.GetModeltemplate(tempMetaData->GetAssetId()), AssetName, FileName);

			ModelTemplate = m_AssetHolder.GetModeltemplate(tempMetaData->GetAssetId());

		}
		else
		{
			FY_CORE_WARN("Found Already Existing ModelTemplate with Asset Name: {0}", AssetName);
		}
		return returnValue;
	}

	bool Assetmanager::AddNewMaterialTemplate(Luna::Material*& Material, const std::string& AssetName, const std::string& FileName)
	{
		bool returnValue = false;

		if (!MaterialLoaded(AssetName))
		{
			returnValue = true;

			m_Material_Asset_Name_Vector.emplace_back(AssetName);

			AssetMetaData<Luna::Material>* tempMetaData = &m_MAT_MetaData_Map[AssetName];

			tempMetaData->SetRefAllData(*m_AssetHolder.GetMaterial(tempMetaData->GetAssetId()), AssetName, FileName);

			Material = m_AssetHolder.GetMaterial(tempMetaData->GetAssetId());

		}
		else
		{
			FY_CORE_WARN("Found Already Existing Material with Asset Name: {0}", AssetName);
		}
		return returnValue;
	}

	AssetMetaData<ModelTemplate> * Assetmanager::GetModeltemplateMetaData(const std::string& AssetName)
	{
		if (ModelTemplateLoaded(AssetName))
		{
			return &m_MT_MetaData_Map[AssetName];
		}
		else
		{
			FY_CORE_WARN("Tried to fetch a non loaded ModelTemplate with AssetName: {0}", AssetName);
		}
		return nullptr;
	}

	AssetMetaData<Luna::Material> const* Assetmanager::GetMaterialMetaData(const std::string& AssetName)
	{
		if (MaterialLoaded(AssetName))
		{
			return &m_MAT_MetaData_Map[AssetName];
		}
		else
		{
			FY_CORE_WARN("Tried to fetch a non loaded material with AssetName: {0}", AssetName);
		}
		return nullptr;
	}



	bool Assetmanager::LinkModelKey(const std::string& AssetName, KeyLabel<ModelTemplate>* In_Key)
	{

		bool returnValue = false;

		if (ModelTemplateLoaded(AssetName)) {
			In_Key->SetKeyData(m_MT_MetaData_Map[AssetName]);
			returnValue = true;
		//	FY_CORE_WARN("Not initialized");
		}

		return returnValue;
	}

	bool Assetmanager::LinkMaterialKey(const std::string& AssetName, KeyLabel<Luna::Material>* In_Key)
	{
		bool returnValue = false;

		if (ModelTemplateLoaded(AssetName)) {
			In_Key->SetKeyData(m_MAT_MetaData_Map[AssetName]);
			returnValue = true;
		//	FY_CORE_WARN("Not initialized");
		}
		return returnValue;
	}



	bool Assetmanager::FileLoaded(const std::string& FileName)
	{
		bool returnValue = false;

		for (int i = 0; i < m_FileName_Vector.size() && returnValue == false; i++)
		{
			if (m_FileName_Vector.at(i) == FileName)
			{
				returnValue = true;
			}
		}

		
		return returnValue;
	}

	bool Assetmanager::AssetLoaded(const std::string& AssetName)
	{
		bool returnValue = false;

		for (int i = 0; i < m_Material_Asset_Name_Vector.size() && returnValue == false; i++)
		{
			if (m_Material_Asset_Name_Vector.at(i) == AssetName)
			{
				returnValue = true;
			}
		}

		for (int i = 0; i < m_ModelTemplate_Asset_Name_Vector.size() && returnValue == false; i++)
		{
			if (m_ModelTemplate_Asset_Name_Vector.at(i) == AssetName)
			{
				returnValue = true;
			}
		}

		return returnValue;
	}

	bool Assetmanager::MaterialLoaded(const std::string& AssetName)
	{
		bool returnValue = false;

		for (int i = 0; i < m_Material_Asset_Name_Vector.size() && returnValue == false; i++)
		{
			if (m_Material_Asset_Name_Vector.at(i) == AssetName)
			{
				returnValue = true;
			}
		}
		return returnValue;
	}

	bool Assetmanager::ModelTemplateLoaded(const std::string& AssetName)
	{
		bool returnValue = false;
		for (int i = 0; i < m_ModelTemplate_Asset_Name_Vector.size() && returnValue == false; i++)
		{
			if (m_ModelTemplate_Asset_Name_Vector.at(i) == AssetName)
			{
				returnValue = true;
			}
		}

		return returnValue;
	}


	Frosty::Assetmanager::~Assetmanager()
	{
		if (m_Instance != nullptr)
		{
		delete m_Instance;
		}
	}



}