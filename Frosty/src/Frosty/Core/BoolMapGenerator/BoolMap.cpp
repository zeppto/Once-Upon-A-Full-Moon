#include<fypch.hpp>
#include"BoolMap.hpp"


BoolMap::BoolMap(const uint16_t& Width, const uint16_t& Height, const uint8_t PixRatio, std::shared_ptr<bool[]>& Boolmap) : m_PixWidth(Width), m_PixHeight(Height),m_PixCoordRatio(PixRatio), m_BoolMap(Boolmap), m_CoordHeight(Height/ PixRatio), m_CoordWidth(Width / PixRatio)
{
}

BoolMap::BoolMap(const BoolMap& other) : m_PixWidth(other.m_PixWidth), m_PixHeight(other.m_PixHeight), m_BoolMap(other.m_BoolMap), m_PixCoordRatio(other.m_PixCoordRatio), m_CoordWidth(other.m_CoordWidth),m_CoordHeight(other.m_CoordHeight)
{


}

BoolMap& BoolMap::operator=(const BoolMap& other)
{
	if (&other != this)
	{
		m_PixWidth = other.m_PixWidth;
		m_PixHeight = other.m_PixHeight;
		m_BoolMap = other.m_BoolMap;
		m_CoordHeight = other.m_CoordHeight;
		m_CoordWidth = other.m_CoordWidth;
	}
	return *this;
}

const bool& BoolMap::CheckCollition(const glm::vec3& LocalPos) const
{
	bool returnValue = false;
	if (m_CoordWidth >= LocalPos.x && 0 <= LocalPos.x && LocalPos.z >= 0 && LocalPos.z <= m_CoordHeight)
	{
		returnValue =  m_BoolMap[static_cast<unsigned int>((LocalPos.x * (float)m_PixCoordRatio + (LocalPos.z * (unsigned int)m_PixWidth)))];
	}
	else
	{
		FY_CORE_FATAL("Checking outside Boolmap grid!");
	}
	return returnValue;
}
