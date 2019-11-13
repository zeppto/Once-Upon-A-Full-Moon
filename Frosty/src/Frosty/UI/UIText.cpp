#include "fypch.hpp"
#include "UIText.h"
#include "Frosty/DEFINITIONS.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"


namespace Frosty
{
	UIText::UIText(std::string text, std::string font, uint16_t fontSize)
	{
		m_type = TEXT;
		m_Text = text;
		m_Font = font;
		m_FontSize = fontSize;

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
}