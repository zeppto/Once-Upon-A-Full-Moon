#include "fypch.hpp"
#include "UISprite.h"
#include "Glad/glad.h"
#include "Frosty/API/AssetManager.hpp"

namespace Frosty
{
	UISprite::UISprite()
	{
		//Pos_3, Uv_2, Normal_3, Color_4
		m_vertices = FY_NEW float[(3 + 2 + 3 + 4) * 6]{
			-1.0, -1.0, 0.0,	0.0, 0.0,	0.5, 1.0, 0.5,	0.0, 1.0, 0.8, 0.5,
			1.0, -1.0, 0.0,		1.0, 0.0,	0.5, 1.0, 0.5,	0.0, 1.0, 0.8, 0.5,
			1.0, 1.0, 0.0,		1.0, 1.0,	0.5, 1.0, 0.5,	0.0, 1.0, 0.8, 0.5,
			1.0, 1.0, 0.0,		1.0, 1.0,	0.5, 1.0, 0.5,	0.0, 1.0, 0.8, 0.5,
			-1.0, 1.0, 0.0,		0.0, 1.0,	0.5, 1.0, 0.5,	0.0, 1.0, 0.8, 0.5,
			-1.0, -1.0, 0.0,	0.0, 0.0,	0.5, 1.0, 0.5,	0.0, 1.0, 0.8, 0.5
		};
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
}