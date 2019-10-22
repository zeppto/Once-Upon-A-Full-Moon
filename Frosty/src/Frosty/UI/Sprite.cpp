#include "fypch.hpp"
#include "Sprite.h"
#include "Glad/glad.h"
#include "Frosty/API/AssetManager/Assetmanager.hpp"

namespace Frosty
{
	Sprite::Sprite()
	{

	}

	Sprite::~Sprite()
	{

	}

	bool Sprite::Init()
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

	unsigned int Sprite::LoadTexture()
	{
		uint16_t textureID = Assetmanager::GetAssetmanager()->GetTextureMetaData("test")->GetData()->GetBufferID();
		//Assetmanager::GetAssetmanager()->GetTextureMetaData("test")->
		return textureID;
	}

	void Sprite::SetAnchor(glm::vec2 anchor)
	{
		m_Transform.SetAnchor(glm::vec3(anchor, 0));
	}

	void Sprite::SetPosition(glm::vec2 pos)
	{
		m_Transform.SetTranslate(glm::vec3(pos, 0));
	}

	void Sprite::SetScale(glm::vec2 scale)
	{
		m_Transform.SetScale(glm::vec3(scale, 0));
	}

	void Sprite::SetRotate(glm::vec2 rot)
	{
		m_Transform.SetRotate(glm::vec3(rot, 0));
	}

	glm::vec2 Sprite::GetAnchor()
	{
		return glm::vec2(m_Transform.GetAnchor().x, m_Transform.GetAnchor().y);
	}

	glm::vec2 Sprite::GetPosition()
	{
		return glm::vec2(m_Transform.GetTranslate().x, m_Transform.GetTranslate().y);
	}

	glm::vec2 Sprite::GetScale()
	{
		return glm::vec2(m_Transform.GetScale().x, m_Transform.GetScale().y);
	}

	glm::vec2 Sprite::GetRotate()
	{
		return glm::vec2(m_Transform.GetRotate().x, m_Transform.GetRotate().y);
	}

	void Sprite::SetColor(glm::vec4 color)
	{
		this->color = color;
	}

	float* Sprite::GetQuad()
	{
		float stuff = this->vertices[1];
		return vertices;
	}

	int Sprite::GetSize()
	{
		return sizeof(vertices);
	}

	Texture& Sprite::GetTexture()
	{
		return m_Texture;
	}

	Transform& Sprite::GetTransform()
	{
		return m_Transform;
	}

}