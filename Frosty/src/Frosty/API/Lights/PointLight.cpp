#include "fypch.hpp"
#include "PointLight.hpp"

namespace Frosty
{
	//PointLight::PointLight() : LightBase()
	//{
	//	m_Position = { 0.f, 0.f, 0.f };
	//	m_Radius = 2.f;
	//	m_Linear_Quadratic = { 0.35f, 0.44f };
	//}

	PointLight::PointLight(glm::vec3 position, glm::vec4 color, float strength, float radius, glm::vec2 linear_Quadratic)
		: LightBase(color, strength)
	{
		m_Position = position;
		m_Radius = radius;
		m_Linear_Quadratic = linear_Quadratic;
	}
	
	PointLight::PointLight(const PointLight& other) : LightBase(other)
	{
		m_Position = other.m_Position;
		m_Radius = other.m_Radius;
		m_Linear_Quadratic = other.m_Linear_Quadratic;

		FY_CORE_ASSERT(false, "Copy constructor is used in class 'PointLight'.");
	}
	
	PointLight& PointLight::operator=(const PointLight& other)
	{
		if (this != &other)
		{
			LightBase::operator=(other);		// honestly idk		~ W-_-W ~
			m_Position = other.m_Position;
			m_Radius = other.m_Radius;
			m_Linear_Quadratic = other.m_Linear_Quadratic;
			
			FY_CORE_ASSERT(false, "Assignment operator is used in class 'PointLight'.");
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

	void PointLight::Setradius(float radius)
	{
		m_Radius = radius;
	}

	void PointLight::SetLinearQuadratic(glm::vec2 linear_Quadratic)
	{
		m_Linear_Quadratic = linear_Quadratic;
	}

	void PointLight::SetPositionAndRadius(glm::vec4 positionAndRadius)
	{
		m_Position = { positionAndRadius.x, positionAndRadius.y, positionAndRadius.z };
		m_Radius = positionAndRadius.w;
	}
	
	const glm::vec3 PointLight::GetPosition()
	{
		return m_Position;
	}

	const float PointLight::GetRadius()
	{
		return m_Radius;
	}

	const glm::vec2 PointLight::GetLinearQuadratic()
	{
		return m_Linear_Quadratic;
	}

	const glm::vec4 PointLight::GetPositionAndRadius()
	{
		return glm::vec4(m_Position, m_Radius);
	}
	const glm::mat4 PointLight::GetTransform()
	{
		return m_Transform.getModel();
	}
}