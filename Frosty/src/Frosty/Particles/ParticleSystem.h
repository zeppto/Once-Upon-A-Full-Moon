#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
#include "fypch.hpp"
#include "Frosty/DEFINITIONS.hpp"
#include "Frosty/API/AssetManager/KeyLabel.hpp"
#include "Frosty/API/AssetManager/AssetFiles/TextureFile.hpp"

namespace Frosty
{
	struct Particle
	{
		glm::vec3 position;
		glm::vec3 color;
		glm::vec4 startPos;
		glm::vec4 direction;
		float lifetime;
		float speed;
		//int padding[]; //In case padding is needed
	};

	const std::string DEFAULT_PARTICLE_PATH = std::string(FY_TEXTURES_FOLDER_ROOT) + "particle.png";
	const uint32_t MAX_PARTICLE_COUNT = 1000;

	class ParticleSystem
	{
	public:
		ParticleSystem(std::string name = "", std::string texturePath = "");
		~ParticleSystem();

		void Update();

		void SetTexturePath(std::string texPath);
		std::string GetTexturePath() const;

	private:

	public:
		//KeyLabel<TextureFile> m_texture;
		Particle particles[MAX_PARTICLE_COUNT];

	private:
		std::string m_name;
		std::string m_texturePath;
		uint32_t m_particleCount;
		float m_emitRate;

	};
}
#endif // ! PARTICLESYSTEM_H