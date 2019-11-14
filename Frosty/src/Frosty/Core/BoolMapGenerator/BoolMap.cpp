#include<fypch.hpp>
#include"BoolMap.hpp"


BoolMap::BoolMap(const uint16_t& Width, const uint16_t& Height, const uint8_t PixRatio, std::shared_ptr<bool[]>& Boolmap, std::shared_ptr<uint64_t[]> BitMap) : m_PixWidth(Width), m_PixHeight(Height),m_PixCoordRatio(PixRatio), m_BoolMap(Boolmap), m_CoordHeight(Height/ PixRatio), m_CoordWidth(Width / PixRatio) , m_BitMap(BitMap)
{
}


BoolMap::BoolMap(const uint16_t& Width, const uint16_t& Height, const uint8_t PixRatio, std::shared_ptr<uint64_t[]> BitMap) : m_PixWidth(Width), m_PixHeight(Height), m_PixCoordRatio(PixRatio), m_CoordHeight(Height / PixRatio), m_CoordWidth(Width / PixRatio), m_BitMap(BitMap)
{
}

BoolMap::BoolMap(const BoolMap& other) : m_PixWidth(other.m_PixWidth), m_PixHeight(other.m_PixHeight), m_BoolMap(other.m_BoolMap), m_PixCoordRatio(other.m_PixCoordRatio), m_CoordWidth(other.m_CoordWidth),m_CoordHeight(other.m_CoordHeight), m_BitMap(other.m_BitMap)
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
		m_BitMap = other.m_BitMap;
		m_PixCoordRatio = other.m_PixCoordRatio;
	}
	return *this;
}

const bool& BoolMap::CheckCollition(const glm::vec3& LocalPos) const
{
	bool returnValue = false;
	if (m_CoordWidth >= LocalPos.x && 0 <= LocalPos.x && LocalPos.z >= 0 && LocalPos.z <= m_CoordHeight)
	{

		int xx = static_cast<int>((LocalPos.x * m_PixCoordRatio)-1);
		int zz = static_cast<int>(((LocalPos.z * m_PixCoordRatio)-1) * m_PixWidth);

		if (xx < 0)
		{
			xx = 0;
		}
		if (zz < 0)
		{
			zz = 0;
		}

		int pos = xx + zz;

	//	bool test = m_BoolMap[pos];

		int div = static_cast<int>(pos / 64);
		int mod = static_cast<int>(pos % 64);


		uint64_t bitmapVal = m_BitMap[div];

		uint64_t kk = 1;
		uint64_t modVal = kk << (63 - mod) ;


		if (bitmapVal & modVal)
		{
			returnValue = true;
		}

	//	returnValue =  m_BoolMap[pos];
	}
	else
	{
		FY_CORE_FATAL("Checking outside Boolmap grid!");
	}
	return returnValue;
}
