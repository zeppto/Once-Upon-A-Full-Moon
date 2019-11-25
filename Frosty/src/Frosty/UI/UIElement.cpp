#include "fypch.hpp"
#include "UIElement.h"

namespace Frosty {

	UIElement::UIElement()
	{

	}

	UIElement::UIElement(const UIElement& p)
	{
		this->m_Transform = p.m_Transform;
	}

	UIElement::~UIElement()
	{

	}

	UIElement& UIElement::operator=(const UIElement& p)
	{
		this->m_Transform = p.m_Transform;

		return *this;
	}

}
