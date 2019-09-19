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
				*m_AssetHolder.GetModeltemplate(ModelTemplate::GetNumberOfModelTemplates()),
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

	Frosty::Assetmanager::~Assetmanager()
	{
		if (m_Instance != nullptr)
		{
		delete m_Instance;
		}
	}



}