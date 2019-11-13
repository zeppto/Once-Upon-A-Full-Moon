#include <fypch.hpp>
#include "UILayout.hpp"

namespace Frosty
{
	UILayout::UILayout()
	{
		m_TextsCount = 1;
		//m_SpritesCount = 1;

		texts.resize(m_TextsCount);
		//sprites.resize(m_SpritesCount);
	}

	UILayout::UILayout(uint16_t textsCount, uint16_t spritesCount)
	{
		m_TextsCount = textsCount;
		//m_SpritesCount = spritesCount;

		texts.resize(textsCount);
		//sprites.resize(spritesCount);
	}

	void UILayout::SetTextsCount(uint16_t count)
	{
		m_TextsCount = count;
	}

	//void UILayout::SetSpritesCount(uint16_t count)
	//{
	//	m_SpritesCount = count;
	//}

	bool UILayout::AddText(glm::vec2 pos, std::string text, std::string font)
	{
		if (m_SetTexts < m_TextsCount)
		{
			texts[m_SetTexts].SetPosition(pos);
			texts[m_SetTexts].SetText(text);
			m_SetTexts++;
			return true;
		}
		else
		{
			//No more space has been allocated (could still push back but let's avoid that)
			return false;
		}
	}
}