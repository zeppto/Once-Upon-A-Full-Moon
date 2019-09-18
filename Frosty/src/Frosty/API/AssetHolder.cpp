#include"AssetHolder.hpp"
#include<fypch.hpp>

namespace Frosty
{

	AssetHolder::AssetHolder()
	{
	}

	AssetHolder::~AssetHolder()
	{
	}



	ModelTemplate* AssetHolder::GetModeltemplate(uint16_t key)
	{
		return &m_ModelMap[key];
	}

	Luna::Material* AssetHolder::GetMaterial(uint16_t key)
	{
		return &m_MaterialMap[key];
	}

}