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

		if (1) //check if asset already exist
		{
			auto Loader = MotherLoader::GetMotherLoader();
			std::vector<Luna::Material> tempMaterialVector;

			Loader->Loadfile(
				FilePath,
				*m_AssetHolder.GetModeltemplate(0),
				tempMaterialVector
				);



			if (1)// check if material exists
			{

				// Temp Function for testing
				for (int i = 0; i < tempMaterialVector.size(); i++)
				{
					*m_AssetHolder.GetMaterial(m_NrOfMaterial++) = tempMaterialVector.at(i);
				}
			}


		}


		return true;
	}

	bool Assetmanager::LoadFile(std::string FilePath, const std::string& set_Prefabkey)
	{
		return false;
	}

	void Assetmanager::AddNewModelTemplate(ModelTemplate* ModelTemplate, const std::string& FileName)
	{



	}

	ModelTemplate* Assetmanager::GetModelTemplate()
	{
		return m_AssetHolder.GetModeltemplate(0);
	}

	bool Assetmanager::FileLoaded(std::string FileName)
	{
		bool returnValue = false;

		for (int i = 0; i < m_Material_FileName_Vector.size() && returnValue == false; i++)
		{
			if (m_Material_FileName_Vector.at(i) == FileName)
			{
				returnValue = true;
			}
		}


		for (int i = 0; i < m_Model_FileName_Vector.size() && returnValue == false; i++)
		{
			if (m_Model_FileName_Vector.at(i) == FileName)
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