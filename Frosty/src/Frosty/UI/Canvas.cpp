#include "fypch.hpp"
#include "Canvas.h"
#include "Frosty/API/Assetmanager.hpp"

namespace Frosty
{
	Canvas::Canvas(glm::vec2 size)
	{
		//this->anchor = glm::vec2(size.x / 2, size.y / 2);
		//this->pos = glm::vec2(size.x / 2, size.y / 2);
		//this->size = glm::vec2(size.x, size.y);
		
		 

		m_Transform.SetScale(glm::vec3(size.x, size.y, 0));		
		
		//Test
		//myUI
		UIText myUI("Created in Canvas", "Gabriola", 50);
		m_Elements.push_back(myUI);
				
		// Test
		// UI Bar
		UiBar m_UiBar("frame2", "UiBar_Texture");		
		AddElement(m_UiBar);
	}

	Canvas::~Canvas()
	{
	}

	void Canvas::LoadTexture()
	{

		//m_Texture.reset(FY_NEW Texture2D("",));
		m_TextureID = AssetManager::GetTexture2D("frame2")->GetRenderID();
	}

	void Canvas::AddElement(UIElement element)
	{
		//Create an UI element and add it to Canvas with AddElement();

		m_Elements.push_back(element);

	}

	void Canvas::SetAnchor(glm::vec2 anchor)
	{
		glm::vec2 offset = glm::vec2(0);
		offset = anchor - glm::vec2(this->m_Transform.GetAnchor().x, this->m_Transform.GetAnchor().y);

		m_Transform.SetAnchor(glm::vec3(anchor, 0));

		for (uint8_t i = 0; i < m_Canvas.size(); i++)
			m_Canvas[i].SetAnchor(offset);
		for (uint8_t i = 0; i < m_Elements.size(); i++)
			m_Elements[i].SetAnchor(offset);
	}

	void Canvas::SetPosition(glm::vec2 pos)
	{
		glm::vec2 offset = glm::vec2(0);
		offset = pos - glm::vec2(this->m_Transform.GetTranslate().x, this->m_Transform.GetTranslate().y);

		m_Transform.SetTranslate(glm::vec3(pos, 0));

		for (uint8_t i = 0; i < m_Canvas.size(); i++)
			m_Canvas[i].SetPosition(offset);
		for (uint8_t i = 0; i < m_Elements.size(); i++)
			m_Elements[i].SetPosition(offset);
	}

	void Canvas::SetScale(glm::vec2 scale)
	{
		glm::vec2 offset = glm::vec2(0);
		offset = scale - glm::vec2(this->m_Transform.GetScale().x, this->m_Transform.GetScale().y);

		m_Transform.SetScale(glm::vec3(scale, 0));

		for (uint8_t i = 0; i < m_Canvas.size(); i++)
			m_Canvas[i].SetScale(offset);
		for (uint8_t i = 0; i < m_Elements.size(); i++)
			m_Elements[i].SetScale(offset);
	}

	void Canvas::SetRotate(glm::vec2 rot)
	{
		glm::vec2 offset = glm::vec2(0);
		offset = rot - glm::vec2(this->m_Transform.GetRotate().x, this->m_Transform.GetRotate().y);

		m_Transform.SetRotate(glm::vec3(rot, 0));

		for (uint8_t i = 0; i < m_Canvas.size(); i++)
			m_Canvas[i].SetRotate(offset);
		for (uint8_t i = 0; i < m_Elements.size(); i++)
			m_Elements[i].SetRotate(offset);
	}

	glm::vec2 Canvas::GetAnchor()
	{
		return glm::vec2(m_Transform.GetAnchor().x, m_Transform.GetAnchor().y);
	}

	glm::vec2 Canvas::GetPosition()
	{
		return glm::vec2(m_Transform.GetTranslate().x, m_Transform.GetTranslate().y);
	}

	glm::vec2 Canvas::GetScale()
	{
		return glm::vec2(m_Transform.GetScale().x, m_Transform.GetScale().y);
	}

	glm::vec2 Canvas::GetRotate()
	{
		return glm::vec2(m_Transform.GetRotate().x, m_Transform.GetRotate().y);
	}

	uint32_t Canvas::GetTexture()
	{
		return m_TextureID;
	}

	Transform& Canvas::GetTransform()
	{
		return m_Transform;// TODO: insert return statement here
	}
}