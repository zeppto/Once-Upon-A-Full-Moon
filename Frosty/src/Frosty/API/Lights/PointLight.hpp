#ifndef POINTLIGHT_HPP
#define POINTLIGHT_HPP

#include "LightBase.hpp"
#include "Frosty/API/Transform.h"

namespace Frosty
{
	class PointLight : public LightBase
	{
	public:
		//PointLight();
		PointLight(glm::vec3 position, glm::vec4 color, float strength, float radius, glm::vec2 linear_Quadratic = { 0.35f, 0.44f });	// Random ass values, probably gonna change it later	~ W-_-W ~
		virtual ~PointLight() {};
		PointLight(const PointLight& other);
		PointLight& operator=(const PointLight& other);
		bool operator==(const PointLight& other);

		// Set
		void SetPosition(glm::vec3 position);
		void Setradius(float radius);
		void SetLinearQuadratic(glm::vec2 linear_Quadratic);
		void SetPositionAndRadius(glm::vec4 positionAndRadius);

		// Get
		const glm::vec3 GetPosition();
		const float GetRadius();
		const glm::vec2 GetLinearQuadratic();
		const glm::vec4 GetPositionAndRadius();
		const glm::mat4 GetTransform();

	private:
		glm::vec3 m_Position;
		float m_Radius;
		glm::vec2 m_Linear_Quadratic;
		Transform m_Transform;
	};
}
#endif