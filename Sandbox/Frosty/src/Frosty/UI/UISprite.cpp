#include "fypch.hpp"
#include "UISprite.h"
#include "Glad/glad.h"
#include"Frosty/API/AssetManager/AssetManager.hpp"


namespace Frosty
{
	UISprite::UISprite()
	{
		//Pos_3, Uv_2, Normal_3, Color_4
		m_vertices = FY_NEW float[(3 + 2 + 3 + 4) * 6]{
			-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,		0.5f, 1.0f, 0.5f,		0.0f, 1.0f, 0.8f, 0.5f,
			 1.0f, -1.0f, 0.0f,		1.0f, 0.0f,		0.5f, 1.0f, 0.5f,		0.0f, 1.0f, 0.8f, 0.5f,
			 1.0f,  1.0f, 0.0f,		1.0f, 1.0f,		0.5f, 1.0f, 0.5f,		0.0f, 1.0f, 0.8f, 0.5f,
			 1.0f,  1.0f, 0.0f,		1.0f, 1.0f,		0.5f, 1.0f, 0.5f,		0.0f, 1.0f, 0.8f, 0.5f,
			-1.0f,  1.0f, 0.0f,		0.0f, 1.0f,		0.5f, 1.0f, 0.5f,		0.0f, 1.0f, 0.8f, 0.5f,
			-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,		0.5f, 1.0f, 0.5f,		0.0f, 1.0f, 0.8f, 0.5f
		};
	}

	UISprite::UISprite(const UISprite& other)
	{
		FY_CORE_ASSERT(false, "Copy constructor in UISprite called.");

		this->m_vertices = FY_NEW float;
		*this->m_vertices = *other.m_vertices;
		this->m_TextureID = other.m_TextureID;
		this->m_Texture = other.m_Texture;
		this->m_VertArray = other.m_VertArray;
	}

	UISprite::~UISprite()
	{
		delete(m_vertices);
	}

	//bool UISprite::Init()
	//{
	//	//try
	//	//{
	//	//	this->m_Texture.id = LoadTexture();
	//	//	this->m_Texture.name = "sprite_Texture";
	//	//	return true;
	//	//}
	//	//catch (const std::exception&)
	//	//{
	//	//	return false;
	//	//}
	//}

	void UISprite::LoadTexture()
	{
		m_TextureID = AssetManager::GetTexture2D("frame2")->GetRenderID();
	}
	UISprite& UISprite::operator=(const UISprite& other)
	{
		FY_CORE_ASSERT(false, "Assignment operator in UISprite called.");

		this->m_vertices = FY_NEW float;
		*this->m_vertices = *other.m_vertices;
		this->m_TextureID = other.m_TextureID;
		this->m_Texture = other.m_Texture;
		this->m_VertArray = other.m_VertArray;

		return *this;
	}
}