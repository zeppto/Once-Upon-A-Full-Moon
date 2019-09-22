#include<fypch.hpp>
#include"AssetHolder.hpp"

namespace Frosty
{

	AssetHolder::AssetHolder()
	{
	}

	AssetHolder::~AssetHolder()
	{
	}



	ModelTemplate* AssetHolder::GetModeltemplate(const uint16_t& AssetId)
	{
		return &m_ModelMap[AssetId];
	}

	Luna::Material* AssetHolder::GetMaterial(const uint16_t& AssetId)
	{
		return &m_MaterialMap[AssetId];
	}

}