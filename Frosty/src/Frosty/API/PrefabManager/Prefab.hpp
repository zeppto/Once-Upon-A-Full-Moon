#ifndef PREFAB_H
#define PREFAB_H 
#include "..//AssetManager/AssetManager.hpp"

namespace Frosty
{

	class Prefab
	{
	public:		//Variables
		

	private:	//Variables
		std::string n_Name;


		uint32_t m_PrefabID = 0;
		static uint32_t s_Unique_ID;
		static uint32_t s_NrOf_Prefabs;
	
		KeyLabel<ModelTemplate> m_ModelKey;
		KeyLabel<Luna::Material> m_MaterialKey;

	public:		//Functions
		Prefab();
		Prefab(Prefab &other);



		~Prefab();

		void SetModelKey(KeyLabel<ModelTemplate> modelKey);
		void SetMaterialKey(KeyLabel<Luna::Material> materialKey);

		KeyLabel<ModelTemplate> GetModelKey();
		KeyLabel<Luna::Material> GetMaterialKey();
		std::string GetName();

		bool SetData(std::string modelName, std::string materialName);

		Prefab& operator = (Prefab& other);
		bool operator == (Prefab& other);

	private:	//Functions

	
	};

}



#endif // !PREFAB

