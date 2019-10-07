#include "fypch.hpp"
#include"PrefabInstance.hpp"


namespace Frosty
{
	uint32_t PrefabInstance::s_Unique_ID = 0;


	PrefabInstance::PrefabInstance(std::string prefabName)
	{
		m_InstanceID = s_Unique_ID++;
		m_PrefabName = prefabName;
	}

	PrefabInstance::PrefabInstance(PrefabInstance& other)
	{
		if (this != &other)
		{
			m_InstanceID = s_Unique_ID++;
			m_Transform = other.m_Transform;
			m_PrefabName = other.m_PrefabName;
		}
	}

	PrefabInstance::~PrefabInstance()
	{

	}

	Transform* PrefabInstance::GetTransform()
	{
		return &m_Transform;
	}

	PrefabInstance& PrefabInstance::operator=(PrefabInstance& other)
	{
		m_InstanceID = s_Unique_ID++;
		m_Transform = other.m_Transform;
		m_PrefabName = other.m_PrefabName;
		return *this;
	}

	bool PrefabInstance::operator==(PrefabInstance& other)
	{
		if (m_PrefabName == other.m_PrefabName)//Might need to add more things later
		{
			return true;
		}
		return false;
	}

}