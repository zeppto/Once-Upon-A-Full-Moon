#include "fypch.hpp"
#include "PrefabManager.h"


namespace Frosty
{
	PrefabManager* PrefabManager::s_Instance = nullptr;
	std::unordered_map<std::string, Prefab> PrefabManager::s_Prefabs;
	
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
	bool PrefabManager::SetPrefab(const std::string& prefabName, const std::string& modelName, const std::string& materialName)
	{
		bool returnValue;
		/*if (CheckIfPrefabExists(prefabName))
		{

		}*/
		returnValue = s_Prefabs[prefabName].SetData(modelName, materialName);
		s_Prefabs[prefabName].SetName(prefabName);

		return returnValue;
	}

	std::unordered_map<std::string, Prefab>* PrefabManager::GetPrefabMap()
	{
		return &s_Prefabs;
	}

	Prefab* PrefabManager::GetPrefab(std::string prefabName)
	{
		return &s_Prefabs.at(prefabName);
	}

	PrefabInstance* PrefabManager::CreatePrefabInstance(std::string prefabName)
	{
		if (CheckIfPrefabExists(prefabName))
		{
			return s_Prefabs.at(prefabName).CreatePrefabInstance();
		}
		FY_CORE_WARN("PrefabManager, Failed to create instance of prefab: {0}", prefabName, ", no prefab with that name exists!");

		return nullptr;
	}

	bool PrefabManager::CheckIfPrefabExists(std::string prefabName)
	{
		for (std::unordered_map<std::string, Prefab>::iterator it = s_Prefabs.begin(); it != s_Prefabs.end(); ++it)
		{
			if (s_Prefabs.at(it->first).GetName() == prefabName)
			{
				return true;
			}
		}

		return false;
	}
}

