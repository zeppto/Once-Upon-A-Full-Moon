#ifndef PREFABMANAGER_H
#define PREFABMANAGER_H
#include "Prefab.hpp"

namespace Frosty
{


	class PrefabManager
	{
	public:		//Variables


	private:	//Variables
		static PrefabManager* s_Instance;

		static std::unordered_map<std::string, Prefab> s_Prefabs;

		friend class Application;

	public:		//Functions
		static PrefabManager* GetPrefabManager();
		~PrefabManager();
		static std::unordered_map<std::string, Prefab>* GetPrefabMap();
		static Prefab* GetPrefab(std::string prefabName);
		static bool SetPrefab(const std::string& prefabName, const std::string& modelName, const std::string& materialName);
		static PrefabInstance* CreatePrefabInstance(std::string prefabName);
		
		
	private:	//Functions

		static bool CheckIfPrefabExists(std::string prefabName);

		inline static void Delete() { if (s_Instance != nullptr) { delete s_Instance; } }
	};


}

#endif // !PREFABMANAGER_H

