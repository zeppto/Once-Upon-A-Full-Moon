#ifndef UI_LAYOUT_HPP
#define UI_LAYOUT_HPP
#include "Frosty/UI/UISprite.h"
#include "Frosty/UI/UIText.h"

namespace Frosty
{
	class UILayout
	{
	public:
		UILayout();
		UILayout(uint16_t textsCount, uint16_t spritesCount);
		
		void SetTextsCount(uint16_t count);
		//void SetSpritesCount(uint16_t count);

		bool AddText(glm::vec2 pos, std::string text, glm::vec3 color = glm::vec3(0.0f), float fontScale = 1.0f, std::string font = "Gabriola");

		std::vector<UIText> texts;
		//std::vector<UISprite> sprites;

	private:
		uint16_t m_TextsCount;
		//uint16_t m_SpritesCount;

		uint16_t m_SetTexts = 0;
		//uint16_t m_SetSprites = 0;
	};

}
#endif // !UI_LAYOUT_HPP