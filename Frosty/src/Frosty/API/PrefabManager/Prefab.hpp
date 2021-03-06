#ifndef PREFAB_H
#define PREFAB_H 
#include "PrefabInstance.hpp"
#include"..\AssetManager\AM.hpp"
#include "Frosty/API/AnimationHandler/AnimationHandler.h"

namespace Frosty
{
	class Prefab
	{
	public:		//Variables
		

	private:	//Variables
		std::string m_PrefabName;

		AnimationHandler m_AnimHandler;

		uint32_t m_PrefabID = 0;
		static uint32_t s_Unique_ID;
		static uint32_t s_NrOf_Prefabs;
	
		KeyLabel<Mesh> m_MeshKey;
		KeyLabel<LinkedMaterial> m_MaterialKey;
		KeyLabel<Animation> m_AnimationKey;
		

		std::vector<PrefabInstance*> m_PrefabInstances;


	public:		//Functions
		Prefab();
		Prefab(Prefab &other);

		~Prefab();

		void SetMeshKey(KeyLabel<Mesh> MeshKey);
		void SetMaterialKey(KeyLabel<LinkedMaterial> materialKey);

		KeyLabel<Mesh> GetMeshKey();
		KeyLabel<LinkedMaterial> GetMaterialKey();
		std::string GetName();

		bool SetData(std::string meshName, std::string materialName);
		void SetName(std::string prefabName);

		PrefabInstance* CreatePrefabInstance();

		PrefabInstance* CreateAnimPrefabInstance();
		AnimationHandler* getAnimationHandler();

		std::vector<PrefabInstance*>* GetInstances();

		Prefab& operator = (Prefab& other);
		bool operator == (Prefab& other);

	private:	//Functions	
	};
}
#endif 
