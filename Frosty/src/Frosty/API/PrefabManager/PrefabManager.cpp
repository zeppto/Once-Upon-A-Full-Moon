#include "fypch.hpp"
#include "PrefabManager.h"

namespace Frosty
{
	
	PrefabManager* PrefabManager::s_Instance = nullptr;


	PrefabManager* Frosty::PrefabManager::GetPrefabManager()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = FY_NEW PrefabManager;
		}

		return s_Instance;
	}

	PrefabManager::~PrefabManager()
	{
	}
	//Modifies the Prefab with the given name. If no prefab exists with that name, a new one will be created.
	bool PrefabManager::setPrefab(const std::string& prefabName, const std::string& modelName, const std::string& materialName)
	{
		bool returnValue;
		/*if (CheckIfPrefabExists(prefabName))
		{
			
		}*/
		returnValue = m_Prefabs[prefabName].SetData(modelName, materialName);

		return returnValue;
	}

	bool PrefabManager::CheckIfPrefabExists(std::string prefabName)
	{
		for (std::unordered_map<std::string, Prefab>::iterator it = m_Prefabs.begin(); it != m_Prefabs.end(); ++it)
		{
			if (m_Prefabs[it->first].GetName() == prefabName)
			{
				return true;
			}
		}

		return false;
	}
}

