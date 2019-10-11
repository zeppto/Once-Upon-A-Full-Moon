#include "fypch.hpp"
#include "ParticleSystem.h"

namespace Frosty
{
	ParticleSystem::ParticleSystem(std::string name, std::string texturePath)
	{
		m_name = name;

		if (texturePath == "")
		{
			m_texturePath = DEFAULT_PARTICLE_PATH;

		}
		else
		{
			m_texturePath = texturePath;
		}

		//m_texture.GetKeyData().SetFilePath(m_texturePath);
	}

	ParticleSystem::~ParticleSystem()
	{

	}

	void ParticleSystem::Update()
	{

	}

	void ParticleSystem::SetTexturePath(std::string texPath)
	{
		m_texturePath = texPath;
	}

	std::string ParticleSystem::GetTexturePath() const
	{
		return m_texturePath;
	}
}
