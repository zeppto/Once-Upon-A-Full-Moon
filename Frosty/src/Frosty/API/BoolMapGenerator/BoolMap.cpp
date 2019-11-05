#include<fypch.hpp>
#include"BoolMap.hpp"

BoolMap::BoolMap(const uint16_t& Width, const uint16_t& Height, std::shared_ptr<bool> Boolmap) : m_Width(Width), m_Height(Height), m_BoolMap(Boolmap)
{
}

BoolMap::BoolMap(const BoolMap& other) : m_Width(other.m_Width), m_Height(other.m_Height), m_BoolMap(other.m_BoolMap)
{
}

BoolMap& BoolMap::operator=(const BoolMap& other)
{
	if (&other != this)
	{
		m_Width = other.m_Width;
		m_Height = other.m_Height;
		m_BoolMap = other.m_BoolMap;
	}
	return *this;
}
