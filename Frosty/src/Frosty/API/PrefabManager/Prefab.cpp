#include "fypch.hpp"
#include "Prefab.hpp"



namespace Frosty
{
	uint32_t Prefab::s_NrOf_Prefabs = 0;
	uint32_t Prefab::s_Unique_ID = 0;

	

	Prefab::Prefab()
	{
		m_PrefabID = s_Unique_ID++;
		s_NrOf_Prefabs++;
	}

	Prefab::Prefab(Prefab& other)
	{
		if (this != &other)
		{
			m_PrefabID = s_Unique_ID++;
			s_NrOf_Prefabs++;

			m_MeshKey = other.m_MeshKey;
			m_MaterialKey = other.m_MaterialKey;
		}
	}

	

	Prefab::~Prefab()
	{
	}

	void Prefab::SetMeshKey(KeyLabel<Mesh> MeshKey)
	{
		m_MeshKey = MeshKey;
	}

	void Prefab::SetMaterialKey(KeyLabel<LinkedMaterial> materialKey)
	{
		m_MaterialKey = materialKey;
	}

	KeyLabel<Mesh> Prefab::GetMeshKey()
	{
		return m_MeshKey;
	}

	KeyLabel<LinkedMaterial> Prefab::GetMaterialKey()
	{
		return m_MaterialKey;
	}
	std::string Prefab::GetName()
	{
		return m_PrefabName;
	}
	bool Prefab::SetData(std::string modelName, std::string materialName)
	{
		bool returnValue = true;

		//if (!Assetmanager::GetAssetmanager()->LinkModelKey(modelName, &m_ModelKey))
		//{
		//	returnValue = false;
		//}

		//if (!Assetmanager::GetAssetmanager()->LinkMaterialKey(materialName, &m_MaterialKey))
		//{
		//	returnValue = false;
		//}

		if (!Assetmanager::LinkKey(modelName, &m_MeshKey))
		{
			returnValue = false;
		}


		if (!Assetmanager::LinkKey(materialName, &m_MaterialKey))
		{
			returnValue = false;
		}

		return returnValue;
	}

	void Prefab::SetName(std::string prefabName)
	{
		m_PrefabName = prefabName;
	}


	PrefabInstance* Prefab::CreatePrefabInstance()
	{
		m_PrefabInstances.emplace_back(FY_NEW PrefabInstance(m_PrefabName));

		return m_PrefabInstances.back();
	}

	std::vector<PrefabInstance*>* Prefab::GetInstances()
	{
		return &m_PrefabInstances;
	}

	Prefab& Prefab::operator=(Prefab& other)
	{
		m_MaterialKey = other.m_MaterialKey;
		m_MeshKey = other.m_MeshKey;
		return *this;
	}
	bool Prefab::operator==(Prefab& other)
	{
		if (m_MeshKey == other.m_MeshKey && m_MaterialKey == other.m_MaterialKey)
		{
			return true;
		}

		return false;
	}
}



