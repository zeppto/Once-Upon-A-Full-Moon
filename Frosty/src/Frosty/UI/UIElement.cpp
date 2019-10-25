#include "fypch.hpp"
#include "UIElement.h"

namespace Frosty {

	UIElement::UIElement()
	{

	}

	UIElement::~UIElement()
	{

	}

	void UIElement::SetAnchor(glm::vec2 anchor)
	{
		m_Transform.SetAnchor(glm::vec3(anchor, 0));
	}

	void UIElement::SetPosition(glm::vec2 pos)
	{
		m_Transform.SetTranslate(glm::vec3(pos, 0));
	}

	void UIElement::SetScale(glm::vec2 scale)
	{
		m_Transform.SetScale(glm::vec3(scale, 0));
	}

	void UIElement::SetRotate(glm::vec2 rot)
	{
		m_Transform.SetRotate(glm::vec3(rot, 0));
	}

	glm::vec2 UIElement::GetAnchor()
	{
		return glm::vec2(m_Transform.GetAnchor().x, m_Transform.GetAnchor().y);
	}

	glm::vec2 UIElement::GetPosition()
	{
		return glm::vec2(m_Transform.GetTranslate().x, m_Transform.GetTranslate().y);
	}

	glm::vec2 UIElement::GetScale()
	{
		return glm::vec2(m_Transform.GetScale().x, m_Transform.GetScale().y);
	}

	glm::vec2 UIElement::GetRotate()
	{
		return glm::vec2(m_Transform.GetRotate().x, m_Transform.GetRotate().y);
	}

}
