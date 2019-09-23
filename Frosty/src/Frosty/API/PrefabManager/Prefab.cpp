#include "fypch.hpp"
#include "Prefab.hpp"

namespace Frosty
{
	

	

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

			m_ModelKey = other.m_ModelKey;
			m_MaterialKey = other.m_MaterialKey;
		}
	}

	

	Prefab::~Prefab()
	{
	}

	void Prefab::SetModelKey(KeyLabel<ModelTemplate> modelKey)
	{
		m_ModelKey = modelKey;
	}

	void Prefab::SetMaterialKey(KeyLabel<Luna::Material> materialKey)
	{
		m_MaterialKey = materialKey;
	}

	KeyLabel<ModelTemplate> Prefab::GetModelKey() 
	{
		return m_ModelKey;
	}

	KeyLabel<Luna::Material> Prefab::GetMaterialKey()
	{
		return m_MaterialKey;
	}
	std::string Prefab::GetName()
	{
		return n_Name;
	}
	bool Prefab::SetData(std::string modelName, std::string materialName)
	{
		bool returnValue = true;

		if (!Assetmanager::GetAssetmanager()->CreateModelKey(modelName, &m_ModelKey))
		{
			returnValue = false;
		}

		if (!Assetmanager::GetAssetmanager()->CreateMaterialKey(materialName, &m_MaterialKey))
		{
			returnValue = false;
		}

		return returnValue;
	}
	Prefab& Prefab::operator=(Prefab& other)
	{
		m_MaterialKey = other.m_MaterialKey;
		m_ModelKey = other.m_ModelKey;
	}
	bool Prefab::operator==(Prefab& other)
	{
		if (m_ModelKey == other.m_ModelKey && m_MaterialKey == other.m_MaterialKey)
		{
			return true;
		}

		return false;
	}
}



