#include <fypch.hpp>
#include "UILayout.hpp"

namespace Frosty
{
	UILayout::UILayout()
	{
		m_TextsCount = 1;
		m_SpritesCount = 1;

		texts.reserve(m_TextsCount);
		sprites.reserve(m_SpritesCount);
	}

	UILayout::UILayout(const UILayout& p)
	{
		this->texts = p.texts;
		this->sprites = p.sprites;

		this->m_TextsCount = p.m_TextsCount;
		this->m_SpritesCount = p.m_SpritesCount;

		this->m_SetTexts = p.m_SetTexts;
		this->m_SetSprites = p.m_SetSprites;
	}

	UILayout::UILayout(uint16_t textsCount, uint16_t spritesCount)
	{
		m_TextsCount = textsCount;
		m_SpritesCount = spritesCount;

		texts.resize(textsCount);
		sprites.resize(spritesCount);
	}

	UILayout& UILayout::operator=(const UILayout& p)
	{
		this->texts = p.texts;
		this->sprites = p.sprites;

		this->m_TextsCount = p.m_TextsCount;
		this->m_SpritesCount = p.m_SpritesCount;

		this->m_SetTexts = p.m_SetTexts;
		this->m_SetSprites = p.m_SetSprites;

		return *this;
	}

	void UILayout::SetTextsCount(uint16_t count)
	{
		m_TextsCount = count;
	}

	void UILayout::SetSpritesCount(uint16_t count)
	{
		m_SpritesCount = count;
	}
	glm::vec2 test;
	bool UILayout::AddText(glm::vec2 pos, std::string text, glm::vec3 color, float fontScale, std::string font)
	{
		if (m_SetTexts < m_TextsCount)
		{
			texts[m_SetTexts].SetPosition(pos);
			test.x = 12;
			test.y = 8734;
			test = texts[m_SetTexts].GetPosition();
			test;
			texts[m_SetTexts].SetText(text);
			texts[m_SetTexts].SetColor(color);
			texts[m_SetTexts].SetFont(font);
			texts[m_SetTexts].SetFontScale(fontScale);
			m_SetTexts++;
			return true;
		}
		else
		{
			//No more space has been allocated (could still push back but let's avoid that)
			return false;
		}
	}

	bool UILayout::AddSprite(glm::vec2 pos, glm::vec2 scale, std::string image, glm::vec4 color) {

		if (m_SetSprites < m_SpritesCount)
		{
			sprites[m_SetSprites].SetTranslateSprite(pos);
			sprites[m_SetSprites].SetScaleSprite(scale);
			sprites[m_SetSprites].SetImage(image);
			sprites[m_SetSprites].SetColorSprite(color);

			m_SetSprites++;
			return true;
		}
		else
		{
			//No more space has been allocated (could still push back but let's avoid that)
			return false;
		}
	}

}