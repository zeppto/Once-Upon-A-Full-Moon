#include "fypch.hpp"
#include "UIText.h"
#include "Frosty/DEFINITIONS.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"


namespace Frosty
{
	UIText::UIText(glm::vec2 position, std::string text, glm::vec3 color, float fontScale, std::string font, uint16_t fontSize)
	{
		m_type = TEXT;
		m_Text = text;
		m_Font = font;
		m_FontSize = fontSize;
		m_Position = position;
		m_Color = color;
		m_FontScale = fontScale;

		m_VertArray.reset(VertexArray::Create());

		std::shared_ptr<VertexBuffer> vertBuffer;
		vertBuffer.reset(VertexBuffer::Create());

		Frosty::BufferLayout layout =
		{
			{ Frosty::ShaderDataType::Float4,	"vertex"	},
		};

		vertBuffer->SetLayout(layout);
		m_VertArray->AddVertexBuffer(vertBuffer); //Add to array
	}

	UIText::~UIText()
	{

	}

	UIText::UIText(const UIText& that)
	{
		m_Font = that.m_Font;
		m_FontSize = that.m_FontSize;
		m_Position = that.m_Position;
		m_Text = that.m_Text;
		m_type = that.m_type;
		m_Color = that.m_Color;
		m_FontScale = that.m_FontScale;

		m_VertArray = that.m_VertArray; //This could possibly be problematic
	}
}