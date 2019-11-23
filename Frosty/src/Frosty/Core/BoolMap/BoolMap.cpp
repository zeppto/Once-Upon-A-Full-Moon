#include<fypch.hpp>
#include"BoolMap.hpp"
#include<fstream>

//BoolMap::BoolMap(const uint16_t& Width, const uint16_t& Height, const uint8_t PixRatio, std::shared_ptr<bool[]>& Boolmap, std::shared_ptr<uint64_t[]> BitMap) : m_PixWidth(Width), m_PixHeight(Height),m_PixCoordRatio(PixRatio), m_BoolMap(Boolmap), m_CoordHeight(Height/ PixRatio), m_CoordWidth(Width / PixRatio) , m_BitMap(BitMap)
//{
//}
//

BoolMap::BoolMap(const uint16_t& Width, const uint16_t& Height, const uint8_t PixRatio, std::shared_ptr<uint64_t[]> BitMap,const uint32_t& BitmapCount) :
	m_FileName(""),
	m_BitMap(BitMap),
	m_PixWidth(Width),
	m_PixHeight(Height), 
	m_PixCoordRatio(PixRatio),
	m_BitMapCount(BitmapCount),
	m_CoordWidth(Width / PixRatio),
	m_CoordHeight(Height / PixRatio)
{}

BoolMap::BoolMap(const BoolMap& other) :
	m_BitMap(other.m_BitMap),
	//m_BoolMap(other.m_BoolMap), 
	m_PixWidth(other.m_PixWidth),
	m_PixHeight(other.m_PixHeight),
	m_BitMapCount(other.m_BitMapCount),
	m_CoordWidth(other.m_CoordWidth),
	m_CoordHeight(other.m_CoordHeight), 
	m_PixCoordRatio(other.m_PixCoordRatio)
{


}

BoolMap& BoolMap::operator=(const BoolMap& other)
{
	if (&other != this)
	{
		m_PixWidth = other.m_PixWidth;
		m_PixHeight = other.m_PixHeight;
	//	m_BoolMap = other.m_BoolMap;
		m_BitMapCount = other.m_BitMapCount;
		m_FileName = other.m_FileName;
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

bool BoolMap::SaveMap(const std::string& FilePath,const std::string& FileName)
{
	bool returnValue = false;

	std::FILE* File;
	//File.open(FilePath + "/" + FileName);

	//std::string filePath = FilePath + "/" + FileName + ".bmap";
	std::string filePath =  FileName + ".bmap";

	File = fopen(filePath.c_str(),"w");
	size_t Test = fwrite(&m_BitMap[0], sizeof(uint64_t), m_BitMapCount, File);
	if (Test == m_BitMapCount)
	{
		returnValue = true;
	}

	fclose(File);

	return returnValue;
}

bool BoolMap::LoadMap(const std::string& FilePath)
{

	bool returnValue = false;

	std::FILE* File;
	//File.open(FilePath + "/" + FileName);

//	std::string filePath = FilePath;



//	int bitSize = std::ceil((texSize / 64.0f));
	std::shared_ptr<uint64_t[]> bitMap(FY_NEW uint64_t[m_BitMapCount]);

	File = fopen(FilePath.c_str(), "r");
	size_t Test = fread(&bitMap[0], sizeof(uint64_t), m_BitMapCount, File);

	if (Test == m_BitMapCount)
	{
		returnValue = true;
	}
	
	fclose(File);
	uint8_t status = remove(FilePath.c_str());


	for (unsigned int i = 0; i < m_BitMapCount; i++)
	{
		if (bitMap[i] != m_BitMap[i])
		{
			int nein = 0;
		}
	}


	return returnValue;

	return false;
}

