#include "fypch.hpp"
#include "DirectionalLight.hpp"

namespace Frosty
{
	//DirectionalLight::DirectionalLight() : LightBase()
	//{
	//	m_Direction = { 0.f, -1.f, 0.f };
	//}

	DirectionalLight::DirectionalLight(glm::vec4 color, float strength, glm::vec3 direction)
		: LightBase(color, strength)
	{
		if (direction == glm::vec3(0.f, 0.f, 0.f))
		{
			//FY_CORE_ERROR("Directional Light Cannot Be 0.0, A Default value will be set");
			m_Direction = { 0.f, -1.f, 0.f };
		}
		else
		{
			m_Direction = glm::normalize(direction);
		}
	}

	DirectionalLight::DirectionalLight(const DirectionalLight& other) 
		: LightBase(other)
	{
		m_Direction = other.m_Direction;

		FY_CORE_ASSERT(false, "Copy constructor is used in class 'DirectionalLight'.");
	}

	DirectionalLight& DirectionalLight::operator=(const DirectionalLight& other)
	{
		if (this != &other)
		{
			LightBase::operator=(other);		// honestly idk		~ W-_-W ~
			m_Direction = other.m_Direction;

			FY_CORE_ASSERT(false, "Assignment operator is used in class 'DirectionalLight'.");
		}
		return *this;
	}

	bool DirectionalLight::operator==(const DirectionalLight& other)
	{
		return LightBase::operator==(other);
	}

	void DirectionalLight::SetDirection(glm::vec3 direction)
	{
		if (direction == glm::vec3(0.f, 0.f, 0.f)) 
		{
			//FY_CORE_ERROR("Directional Light Cannot Be 0.0, A Default value will be set");
			m_Direction = { 0.f, -1.f, 0.f };
		}
		else 
		{
			m_Direction = glm::normalize(direction);
		}
	}
	
	const glm::vec3& DirectionalLight::GetDirection()
	{
		return m_Direction;
	}
}