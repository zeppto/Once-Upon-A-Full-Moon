#include "fypch.hpp"
#include "LightBase.hpp"

namespace Frosty 
{ 
	uint16_t LightBase::s_LightIDCounter = 0; 

	LightBase::LightBase()
	{
		m_Position = { 0.f, 0.f, 0.f };
		m_Color = { 1.f, 1.f, 1.f };
		m_Strength = 1.f;
		m_ID = uint8_t(s_LightIDCounter++);

	}

	LightBase::LightBase(glm::vec3 position, glm::vec3 color, float strength)
	{
		m_Position = position;
		m_Color = color;
		m_Strength = strength;
		m_ID = uint8_t(s_LightIDCounter++);
	}

	LightBase::LightBase(const LightBase& other)
	{
		m_Position = other.m_Position;
		m_Color = other.m_Color;
		m_Strength = other.m_Strength;
		m_ID = s_LightIDCounter++;
	}

	LightBase& Frosty::LightBase::operator=(const LightBase& other)
	{
		if(this != &other)
		{
			m_Position = other.m_Position;
			m_Color = other.m_Color;
			m_Strength = other.m_Strength;
			m_ID = s_LightIDCounter++;
		}
		return *this;
	}

	bool LightBase::operator==(const LightBase& other)
	{
		return m_ID == other.m_ID;
	}
	
	void LightBase::SetPosition(glm::vec3 position)
	{
		m_Position = position;
	}
	
	void LightBase::SetColor(glm::vec3 color)
	{
		m_Color = color;
	}
	
	void LightBase::SetStrength(float strength)
	{
		m_Strength = strength;
	}
	
	const glm::vec3 LightBase::GetPosition()
	{
		return m_Position;
	}
	
	const glm::vec3 LightBase::GetColor()
	{
		return m_Color;
	}
	
	const float LightBase::GetStrength()
	{
		return m_Strength;
	}
	
	const uint8_t LightBase::GetID()
	{
		return m_ID;
	}
}