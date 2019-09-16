#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "LightBase.hpp"

namespace Frosty
{
	class PointLight :
		public LightBase
	{
	private:
		//uint8_t m_Radius;		// Will be implemented later	~ W-_-W ~
		glm::vec2 m_Linear_Quadratic;

	public:
		PointLight();
		PointLight(glm::vec3 position, glm::vec3 color, float strength, glm::vec2 linear_Quadratic = { 0.35f, 0.44f });	// Random ass values, probably gonna change it later	~ W-_-W ~
		virtual ~PointLight() {};
		PointLight(const PointLight& other);
		PointLight& operator=(const PointLight& other);
		bool operator==(const PointLight& other);

		// Set
		void SetLinearQuadratic(glm::vec2 linear_Quadratic);

		// Get
		const glm::vec2 GetLinearQuadratic();
	};
}
#endif