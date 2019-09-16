#include "fypch.hpp"
#include "PointLight.hpp"

namespace Frosty
{
	PointLight::PointLight() : LightBase()
	{
		m_Linear_Quadratic = { 0.35f, 0.44f };
	}

	PointLight::PointLight(glm::vec3 position, glm::vec3 color, float strength, glm::vec2 linear_Quadratic)
		: LightBase(position, color, strength)
	{
		m_Linear_Quadratic = linear_Quadratic;
	}
	
	PointLight::PointLight(const PointLight& other) : LightBase(other)
	{
		m_Linear_Quadratic = other.m_Linear_Quadratic;
	}
	
	PointLight& PointLight::operator=(const PointLight& other)
	{
		if (this != &other)
		{
			LightBase::operator=(other);
			m_Linear_Quadratic = other.m_Linear_Quadratic;
		}
		return *this;
	}

	bool PointLight::operator==(const PointLight& other)
	{
		return LightBase::operator==(other);
	}
	
	void PointLight::SetLinearQuadratic(glm::vec2 linear_Quadratic)
	{
		m_Linear_Quadratic = linear_Quadratic;
	}
	
	const glm::vec2 PointLight::GetLinearQuadratic()
	{
		return m_Linear_Quadratic;
	}
}