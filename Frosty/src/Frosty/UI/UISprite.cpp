#include "fypch.hpp"
#include "UISprite.h"
#include "Glad/glad.h"
#include"Frosty/API/AssetManager/AssetManager.hpp"


namespace Frosty
{
	UISprite::UISprite(glm::vec2 pos, glm::vec2 scale, std::string image, glm::vec4 color)
	{
		m_type = TYPE::TEXT;
		m_Translate = pos;
		m_Scale = scale;
		m_Image = image;
		m_Color = color;

		glm::mat4 tempTransform, tempScale, tempRotate;

		tempTransform = glm::translate(glm::mat4(1.0f), glm::vec3(m_Translate, 0.0f));
		tempScale = glm::scale(m_Transform, glm::vec3(m_Scale.x * 50, m_Scale.y * 50, 1.0f));
		tempRotate = glm::rotate(m_Transform, glm::radians(45.0f), glm::vec3(0.5f, 0.5f, 0.0f));

		m_Transform = tempTransform + tempScale + tempRotate;

		m_TextureID = Frosty::AssetManager::GetTexture2D(image)->GetRenderID();
		m_VertArray = Frosty::AssetManager::GetMesh("pCube1");
	}

	UISprite::UISprite(const UISprite& other)
	{

		this->m_Translate = other.m_Translate;
		this->m_Scale = other.m_Scale;
		this->m_Image = other.m_Image;
		this->m_Color = other.m_Color;
		this->m_Transform = other.m_Transform;

		this->m_TextureID = other.m_TextureID;
		this->m_VertArray = other.m_VertArray;
	}

	UISprite::~UISprite()
	{
		/*

		//Pos_3, Uv_2, Normal_3, Color_4
		m_vertices = FY_NEW float[(3 + 2 + 3 + 4) * 6]{
			-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,		0.5f, 1.0f, 0.5f,		0.0f, 1.0f, 0.8f, 0.5f,
			 1.0f, -1.0f, 0.0f,		1.0f, 0.0f,		0.5f, 1.0f, 0.5f,		0.0f, 1.0f, 0.8f, 0.5f,
			 1.0f,  1.0f, 0.0f,		1.0f, 1.0f,		0.5f, 1.0f, 0.5f,		0.0f, 1.0f, 0.8f, 0.5f,
			 1.0f,  1.0f, 0.0f,		1.0f, 1.0f,		0.5f, 1.0f, 0.5f,		0.0f, 1.0f, 0.8f, 0.5f,
			-1.0f,  1.0f, 0.0f,		0.0f, 1.0f,		0.5f, 1.0f, 0.5f,		0.0f, 1.0f, 0.8f, 0.5f,
			-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,		0.5f, 1.0f, 0.5f,		0.0f, 1.0f, 0.8f, 0.5f
		};


		{
			glm::translate(glm::mat4(1.0f), glm::vec3(m_Position, 1.0f));
			m_Position = pos;

		*/
	}

	UISprite& UISprite::operator=(const UISprite& other)
	{
		//FY_CORE_ASSERT(false, "Assignment operator in UISprite called.");

		this->m_Translate = other.m_Translate;
		this->m_Scale = other.m_Scale;
		this->m_Image = other.m_Image;
		this->m_Color = other.m_Color;
		this->m_Transform = other.m_Transform;

		this->m_TextureID = other.m_TextureID;
		this->m_VertArray = other.m_VertArray;

		return *this;
	}
}