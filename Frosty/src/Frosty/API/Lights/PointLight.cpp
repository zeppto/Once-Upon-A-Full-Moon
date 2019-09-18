#include "fypch.hpp"
#include "PointLight.hpp"

namespace Frosty
{
	PointLight::PointLight() : LightBase()
	{
		m_Position = { 0.f, 0.f, 0.f };
		m_Linear_Quadratic = { 0.35f, 0.44f };
	}

	PointLight::PointLight(glm::vec3 position, glm::vec4 color, float strength, glm::vec2 linear_Quadratic)
		: LightBase(color, strength)
	{
		m_Position = position;
		m_Linear_Quadratic = linear_Quadratic;
	}
	
	PointLight::PointLight(const PointLight& other) : LightBase(other)
	{
		m_Position = other.m_Position;
		m_Linear_Quadratic = other.m_Linear_Quadratic;
	}
	
	PointLight& PointLight::operator=(const PointLight& other)
	{
		if (this != &other)
		{
			LightBase::operator=(other);
			m_Position = other.m_Position;
			m_Linear_Quadratic = other.m_Linear_Quadratic;
		}
		return *this;
	}

	bool PointLight::operator==(const PointLight& other)
	{
		return LightBase::operator==(other);
	}
	
	void PointLight::SetPosition(glm::vec3 position)
	{
		m_Position = position;
	}

	void PointLight::SetLinearQuadratic(glm::vec2 linear_Quadratic)
	{
		m_Linear_Quadratic = linear_Quadratic;
	}
	
	const glm::vec3 PointLight::GetPosition()
	{
		return m_Position;
	}

	const glm::vec2 PointLight::GetLinearQuadratic()
	{
		return m_Linear_Quadratic;
	}
}