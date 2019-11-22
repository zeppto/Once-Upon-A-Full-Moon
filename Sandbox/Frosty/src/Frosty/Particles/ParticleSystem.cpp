#include "fypch.hpp"
#include "ParticleSystem.h"

namespace Frosty
{
	ParticleSystem::ParticleSystem(std::string name, std::string texturePath, size_t particleCount)
	{
		m_name = name;
		m_particleCount = particleCount;
		m_modelMat = glm::mat4(1.0f);

		m_particleVertArray.reset(VertexArray::Create());

		std::shared_ptr<VertexBuffer> vertBuffer;
		vertBuffer.reset(VertexBuffer::Create(particles, sizeof(particles)));

		BufferLayout layout =
		{
			{ ShaderDataType::Float4,	"pos" },
			//{ ShaderDataType::Float4,	"color" },
			//{ ShaderDataType::Float4,	"startPos" },
			//{ ShaderDataType::Float4,	"dir" },
			//{ ShaderDataType::Float,	"lifetime" },
			//{ ShaderDataType::Float,	"speed" }
		};

		vertBuffer->SetLayout(layout);
		vertBuffer->SetNrOfVertices(m_particleCount);
		m_particleVertArray->AddVertexBuffer(vertBuffer); //Add to array

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

	void ParticleSystem::LoadTexture()
	{
		m_textureID = Assetmanager::GetAssetmanager()->GetTextureMetaData("particle")->GetData()->GetBufferID();
	}

	void ParticleSystem::SetTexturePath(std::string texPath)
	{
		m_texturePath = texPath;
	}

	std::string ParticleSystem::GetTexturePath() const
	{
		return m_texturePath;
	}

	void ParticleSystem::SetParticleCount(size_t count)
	{
		m_particleCount = count;
	}

	size_t ParticleSystem::GetParticleCount() const
	{
		return m_particleCount;
	}

	std::shared_ptr<VertexArray>& ParticleSystem::GetVertexArray()
	{
		return m_particleVertArray;
	}

	glm::mat4 ParticleSystem::GetModelMatrix()
	{
		return m_modelMat;
	}

	uint16_t& ParticleSystem::GetTextureID()
	{
		return m_textureID;
	}
}
