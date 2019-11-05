#include "fypch.hpp"
#include "UIBar.hpp"
#include"Frosty/API/AssetManager/AM.hpp"
//#include "Frosty/API/Assetmanager.hpp"

namespace Frosty
{
	Frosty::UiBar::UiBar(std::string filename, std::string name)
	{
		m_Type = BAR;
		//m_Texture.id = SetTexture(filename);
		//m_Texture.name = name;
	}

	UiBar::~UiBar()
	{
	}
	
	void UiBar::SetTexture(std::string filename)
	{
		m_Texture = AssetManager::GetTexture2D(filename)->GetRenderID();
	}
	
	uint32_t UiBar::GetTexture()
	{
		return m_Texture;
	}
}
