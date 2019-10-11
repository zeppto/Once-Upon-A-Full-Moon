#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
#include "fypch.hpp"
#include "Frosty/DEFINITIONS.hpp"
#include "Frosty/API/AssetManager/KeyLabel.hpp"
#include "Frosty/API/AssetManager/AssetFiles/TextureFile.hpp"
#include "Frosty/RenderEngine/VertexArray.hpp"
#include "Frosty/API/AssetManager/Assetmanager.hpp"

namespace Frosty
{
	struct Particle
	{
		glm::vec4 position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		glm::vec4 color = glm::vec4(1.0);
		glm::vec4 startPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		glm::vec4 direction = glm::vec4(0.0, 1.0, 0.0, 1.0);;
		float lifetime = 2.0f;
		float speed = 2.0f;
		//int padding[]; //In case padding is needed
	};

	const std::string DEFAULT_PARTICLE_PATH = std::string(FY_TEXTURES_FOLDER_ROOT) + "particle.png";
	const uint32_t MAX_PARTICLE_COUNT = 1000;

	class ParticleSystem
	{
	public:
		ParticleSystem(std::string name, std::string texturePath, size_t particleCount);
		~ParticleSystem();

		void Update();

		void LoadTexture();

		void SetTexturePath(std::string texPath);
		std::string GetTexturePath() const;

		void SetParticleCount(size_t count);
		size_t GetParticleCount() const;

		std::shared_ptr<VertexArray>& GetVertexArray();

		glm::mat4 GetModelMatrix();
		uint16_t& GetTextureID();

	private:

	public:
		//KeyLabel<TextureFile> m_texture;
		Particle particles[MAX_PARTICLE_COUNT];
		std::shared_ptr<VertexArray> m_particleVertArray;

	private:
		std::string m_name;
		std::string m_texturePath;
		uint32_t m_particleCount;
		float m_emitRate;
		glm::mat4 m_modelMat;

		uint16_t m_textureID;

	};
}
#endif // ! PARTICLESYSTEM_H