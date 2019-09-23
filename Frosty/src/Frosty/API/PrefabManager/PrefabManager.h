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

		std::unordered_map<std::string, Prefab> m_Prefabs;
		

	public:		//Functions
		static PrefabManager* GetPrefabManager();
		~PrefabManager();
		bool setPrefab( std::string& prefabName, const std::string& modelName, const std::string& materialName);
	private:	//Functions

		bool CheckIfPrefabExists(std::string prefabName);
	};

	
}

#endif // !PREFABMANAGER_H

