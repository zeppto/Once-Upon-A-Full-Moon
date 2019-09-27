#include "fypch.hpp"
#include "Sprite.h"
#include "Glad/glad.h"

namespace Frosty
{
	Sprite::Sprite()
	{
		this->m_TextureID = 0;
		this->m_QuadVbo = 0;
		vertices;
	}

	Sprite::~Sprite()
	{
	}

	bool Sprite::Init()
	{
		return m_TextureID = LoadTexture();;
	}

	unsigned int Sprite::LoadTexture()
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		/*unsigned char* data = new unsigned char[4];*/
		glm::vec4 data { 0 };

		// Allocate the needed space.
		int width = 1;
		int height = 1;

		data[0] = color.x * 255;	//Red
		data[1] = color.y * 255;	//Green
		data[2] = color.z * 255;	//Blue
		data[3] = color.w * 255;	//Alpha

		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data);
		glGenerateMipmap(GL_TEXTURE_2D);

		return textureID;
	}

	float *Sprite::GetQuad()
	{
		float stuff = this->vertices[1];
		return vertices;
	}

	int Sprite::GetSize()
	{
		return sizeof(vertices);
	}

	unsigned int Sprite::GetTexureID()
	{
		return m_TextureID;
	}

}