#ifndef PREFABMANAGER_H
#define PREFABMANAGER_H
#include "Prefab.hpp"
#include"PrefabInstance.hpp"

namespace Frosty
{


	class PrefabManager
	{
	public:		//Variables


	private:	//Variables
		static PrefabManager* s_Instance;

		std::unordered_map<std::string, Prefab> m_Prefabs;



		friend class Assetmanager;

	public:		//Functions
		static PrefabManager* GetPrefabManager();
		~PrefabManager();
		std::unordered_map<std::string, Prefab>* GetPrefabMap();
		Prefab* GetPrefab(std::string prefabName);
		bool setPrefab(const std::string& prefabName, const std::string& modelName, const std::string& materialName);
		std::unordered_map<std::string, Prefab>* GetPrefabMap();
		Prefab* GetPrefab(std::string prefabName);

		PrefabInstance* CreatePrefabInstance(std::string prefabName);
		
	private:	//Functions

		bool CheckIfPrefabExists(std::string prefabName);

		inline static void Delete() { if (s_Instance != nullptr) { delete s_Instance; } }
	};


}

#endif // !PREFABMANAGER_H

