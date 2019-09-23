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
		m_ModelKey = modelKey
	}

	void Prefab::SetMaterialKey(KeyLabel<Luna::Material> materialKey)
	{
		m_MaterialKey = materialKey;
	}

	KeyLabel<ModelTemplate> Frosty::Prefab::GetModelKey()
	{
		return m_ModelKey;
	}

	KeyLabel<Luna::Material> Frosty::Prefab::GetMaterialKey()
	{
		return m_MaterialKey;
	}
	bool Prefab::SetData(std::string modelName, std::string materialName)
	{
		bool returnValue = true;

		if (!Assetmanager::GetAssetmanager()->CreateModelKey(std::string, &m_ModelKey))
		{
			returnValue = false;
		}

		if (!Assetmanager::GetAssetmanager()->CreateMaterialKey(std::string, &m_MaterialKey))
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



