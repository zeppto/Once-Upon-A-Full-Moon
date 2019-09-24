#include<fypch.hpp>
#include"AssetHolder.hpp"

namespace Frosty
{

	AssetHolder::AssetHolder()
	{
		m_Container_Slot_Counter = 0;
	}

	AssetHolder::~AssetHolder()
	{
	}



	ModelTemplate* AssetHolder::GetModeltemplate(const int32_t& AssetId)
	{
		return &m_ModelMap[AssetId];
	}

	Luna::Material* AssetHolder::GetMaterial(const int32_t& AssetId)
	{
		return &m_MaterialMap[AssetId];
	}

}