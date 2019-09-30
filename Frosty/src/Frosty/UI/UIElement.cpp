#include "fypch.hpp"
#include "UIElement.h"

Frosty::UIElement::UIElement()
{

}

Frosty::UIElement::~UIElement()
{

}

void Frosty::UIElement::setPosition(glm::vec2 pos)
{
	m_position = pos;
}

glm::vec2 Frosty::UIElement::getPosition() const
{
	return m_position;
}
