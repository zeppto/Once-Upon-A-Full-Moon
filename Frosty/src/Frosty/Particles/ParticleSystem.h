#ifndef  PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
#include "fypch.hpp"

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

	class ParticleSystem
	{
	public:
		ParticleSystem();
		~ParticleSystem();

		void Update();

	private:

	public:

	private:
		uint32_t m_particleCount;
		float m_emitRate;

	};
}
#endif // ! PARTICLESYSTEM_H