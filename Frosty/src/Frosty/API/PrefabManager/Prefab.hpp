#ifndef PREFAB_H
#define PREFAB_H 
#include "..//AssetManager/AssetManager.hpp"
#include "PrefabInstance.hpp"

namespace Frosty
{

	class Prefab
	{
	public:		//Variables
		

	private:	//Variables
		std::string m_PrefabName;


		uint32_t m_PrefabID = 0;
		static uint32_t s_Unique_ID;
		static uint32_t s_NrOf_Prefabs;
	
		KeyLabel<ModelTemplate> m_ModelKey;
		KeyLabel<LinkedMaterial> m_MaterialKey;

		//std::vector<PrefabInstance*> m_PrefabInstances;
		TripleLinkedList<PrefabInstance*> m_PrefabInstances;


	public:		//Functions
		Prefab();
		Prefab(Prefab &other);

		~Prefab();

		void SetModelKey(KeyLabel<ModelTemplate> modelKey);
		void SetMaterialKey(KeyLabel<LinkedMaterial> materialKey);

		KeyLabel<ModelTemplate> GetModelKey();
		KeyLabel<LinkedMaterial> GetMaterialKey();
		std::string GetName();

		bool SetData(std::string modelName, std::string materialName);
		void SetName(std::string prefabName);
		
		PrefabInstance* CreatePrefabInstance();
		
		TripleLinkedList<PrefabInstance*>* GetInstances();

		Prefab& operator = (Prefab& other);
		bool operator == (Prefab& other);

	private:	//Functions

	
	};

}



#endif // !PREFAB

