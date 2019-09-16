#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "LightBase.hpp"

namespace Frosty
{
	class DirectionalLight 
		: public LightBase
	{
	private:
		glm::vec3 m_Direction;

	public:
		DirectionalLight();
		DirectionalLight(glm::vec3 position, glm::vec3 color, float strength, glm::vec3 direction = { 0.f, -1.f, 0.f });
		virtual ~DirectionalLight() {};
		DirectionalLight(const DirectionalLight& other);
		DirectionalLight& operator=(const DirectionalLight& other);
		bool operator==(const DirectionalLight& other);

		// Set
		void SetDirection(glm::vec3 direction);

		//Get
		const glm::vec3& GetDirection();
	};
}
#endif