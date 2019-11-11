#include<fypch.hpp>
#include"BoolMap.hpp"


BoolMap::BoolMap(const uint16_t& Width, const uint16_t& Height, const uint8_t PixRatio, std::shared_ptr<bool[]>& Boolmap) : m_Width(Width), m_Height(Height),m_PixCoordRatio(PixRatio), m_BoolMap(Boolmap)
{
}

BoolMap::BoolMap(const BoolMap& other) : m_Width(other.m_Width), m_Height(other.m_Height), m_BoolMap(other.m_BoolMap), m_PixCoordRatio(other.m_PixCoordRatio)
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

const bool& BoolMap::CheckCollition(const glm::vec3& LocalPos) const
{
	glm::vec3 temp = LocalPos * (float)m_PixCoordRatio;

	float xx = LocalPos.x * (float)m_PixCoordRatio;
	float zz = LocalPos.z * (float)m_PixCoordRatio * m_Width;

	int pp = static_cast<int>(xx + zz);
	bool val = m_BoolMap[pp];

	//int tempSize = m_Height * (float)m_PixCoordRatio * m_Width * (float)m_PixCoordRatio;

	//int tempX = static_cast<int>( temp.x);
	//int tempZ = static_cast<int>( temp.z);

	//for (int i = 0; i < tempSize; i++)
	//{
	//	if (m_BoolMap[i])
	//	{
	//		int o = 0;
	//	}
	//}

	return m_BoolMap[pp];
}
