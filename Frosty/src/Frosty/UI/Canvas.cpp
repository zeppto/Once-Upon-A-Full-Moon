#include "fypch.hpp"
#include "Canvas.h"
#include "Frosty/API/AssetManager/Assetmanager.hpp"

namespace Frosty
{

	Canvas::Canvas(glm::vec2 size)
	{
		//this->anchor = glm::vec2(size.x / 2, size.y / 2);
		//this->pos = glm::vec2(size.x / 2, size.y / 2);
		//this->size = glm::vec2(size.x, size.y);

		m_Transform.SetTranslate(glm::vec3(size.x / 2, size.y / 2, 0));
		m_Transform.SetScale(glm::vec3(size.x, size.y, 0));

		//Test
	}

	Canvas::~Canvas()
	{
	}

	bool Canvas::Init()
	{
		try
		{
			this->m_Texture.id = LoadTexture();
			this->m_Texture.name = "sprite_Texture";
			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}

	unsigned int Canvas::LoadTexture()
	{
		uint16_t textureID = Assetmanager::GetAssetmanager()->GetTextureMetaData("test")->GetData()->GetBufferID();
		//Assetmanager::GetAssetmanager()->GetTextureMetaData("test")->
		return textureID;
	}

	void Canvas::SetAnchor(glm::vec2 anchor)
	{
		m_Transform.SetAnchor(glm::vec3(anchor, 0));
	}

	void Canvas::SetPosition(glm::vec2 pos)
	{
		glm::vec2 offset = glm::vec2(0);
		offset = pos - glm::vec2(this->m_Transform.GetTranslate().x, this->m_Transform.GetTranslate().y);

		m_Transform.SetTranslate(glm::vec3(pos, 0));

		for (int i = 0; i < m_Canvas.size(); i++)
			m_Canvas[i].SetPosition(offset);
		for (int i = 0; i < m_Elements.size(); i++)
			m_Elements[i].SetPosition(offset);
		for (int i = 0; i < m_Sprite.size(); i++)
			m_Sprite[i].SetPosition(offset);
	}

	void Canvas::SetScale(glm::vec2 scale)
	{
		m_Transform.SetScale(glm::vec3(scale, 0));
	}

	void Canvas::SetRotate(glm::vec2 rot)
	{
		m_Transform.SetRotate(glm::vec3(rot, 0));
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

	uint16_t Canvas::GetTexture()
	{
		return m_Texture.id;
	}

	Transform& Canvas::GetTransform()
	{
		return m_Transform;// TODO: insert return statement here
	}

	float* Canvas::GetQuad()
	{
		return m_Vertices;
	}


}