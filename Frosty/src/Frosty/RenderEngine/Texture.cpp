#include <fypch.hpp>
#include "Texture.hpp"
#include "Renderer.hpp"

#include <stb_image.h>
#include <glad/glad.h>

namespace Frosty
{
	Texture2D::Texture2D(const std::string& name, const std::string& path)
		: m_Name(name), m_Path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		FY_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void Texture2D::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}
	
	void Texture2D::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
