#include<fypch.hpp>
#include"BoolMap.hpp"
#include<fstream>
//#include"AssetFile.hpp"

namespace Frosty
{

	//BoolMap::BoolMap(const uint16_t& Width, const uint16_t& Height, const uint8_t PixRatio, std::shared_ptr<bool[]>& Boolmap, std::shared_ptr<uint64_t[]> BitMap) : m_PixWidth(Width), m_PixHeight(Height),m_PixCoordRatio(PixRatio), m_BoolMap(Boolmap), m_CoordHeight(Height/ PixRatio), m_CoordWidth(Width / PixRatio) , m_BitMap(BitMap)
	//{
	//}
	//

	BoolMap::BoolMap(const FileMetaData& MetaData) : 
		m_FileName(""),
		m_BitMap(nullptr),
		m_PixWidth(0),
		m_PixHeight(0),
		m_PixCoordRatio(0),
		m_BitMapCount(0),
		m_CoordWidth(0),
		m_CoordHeight(0),
		AssetFile(MetaData)
	{
	}

	BoolMap::BoolMap(const uint16_t& Width, const uint16_t& Height, const uint8_t PixRatio, std::shared_ptr<uint64_t[]> BitMap, const uint32_t& BitmapCount) :
		m_FileName(""),
		m_BitMap(BitMap),
		m_PixWidth(Width),
		m_PixHeight(Height),
		m_PixCoordRatio(PixRatio),
		m_BitMapCount(BitmapCount),
		m_CoordWidth(Width / PixRatio),
		m_CoordHeight(Height / PixRatio),
		AssetFile(FileMetaData())
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

	const bool& BoolMap::CheckCollision(const glm::vec3& LocalPos) const
	{
		bool returnValue = false;
		if (m_CoordWidth >= LocalPos.x && 0 <= LocalPos.x && LocalPos.z >= 0 && LocalPos.z <= m_CoordHeight)
		{
			//To do: Check offset when bool map is implemented
			int xx = static_cast<int>((LocalPos.x * m_PixCoordRatio) - 1);
			int zz = static_cast<int>(((LocalPos.z * m_PixCoordRatio) - 1) * m_PixWidth);

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
			uint64_t modVal = kk << (63 - mod);


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

	bool BoolMap::SaveMap(const std::string& FilePath, const std::string& FileName)
	{
		bool returnValue = false;

		std::FILE* File;
		//File.open(FilePath + "/" + FileName);

		//std::string filePath = FilePath + "/" + FileName + ".bmap";
		std::string filePath = FileName + ".bmap";

		File = fopen(filePath.c_str(), "w");

		size_t TestWidth = fwrite(&m_CoordWidth, sizeof(uint16_t), 1, File);
		size_t TestHeigth = fwrite(&m_CoordHeight, sizeof(uint16_t), 1, File);
		size_t TestPixCoordRatio = fwrite(&m_PixCoordRatio, sizeof(uint8_t), 1, File);
		size_t TestBitMapCount = fwrite(&m_BitMapCount, sizeof(uint32_t), 1, File);

		size_t TestBitMap = fwrite(&m_BitMap[0], sizeof(uint64_t), m_BitMapCount, File);

		if (TestBitMap == m_BitMapCount)
		{
			returnValue = true;
		}

		fclose(File);

		return returnValue;
	}

	bool BoolMap::LoadMap(const std::string& FilePath)
	{

	//	bool returnValue = false;

	//	std::FILE* File;
	//	//File.open(FilePath + "/" + FileName);

	////	std::string filePath = FilePath;



	////	int bitSize = std::ceil((texSize / 64.0f));
	//	std::shared_ptr<uint64_t[]> bitMap(FY_NEW uint64_t[m_BitMapCount]);

	//	File = fopen(FilePath.c_str(), "r");
	//	size_t Test = fread(&bitMap[0], sizeof(uint64_t), m_BitMapCount, File);

	//	if (Test == m_BitMapCount)
	//	{
	//		returnValue = true;
	//	}
	//	//
	//	//fclose(File);
	//	//uint8_t status = remove(FilePath.c_str());


	//	//for (int i = 0; i < m_BitMapCount; i++)
	//	//{
	//	//	if (bitMap[i] != m_BitMap[i])
	//	//	{
	//	//		int nein = 0;
	//	//	}
	//	//}


	//	return returnValue;

		return false;
	}

	bool BoolMap::LoadToMem(const bool& Reload)
	{
		bool returnValue = false;



		std::FILE* File;

	//	int bitSize = std::ceil((texSize / 64.0f));


		File = fopen(m_MetaData.FullFilePath.c_str(), "r");

		if (File != nullptr)
		{
			size_t TestWidth = fread(&m_CoordWidth, sizeof(uint16_t), 1, File);
			size_t TestHeigth = fread(&m_CoordHeight, sizeof(uint16_t), 1, File);
			size_t TestPixCoordRatio = fread(&m_PixCoordRatio, sizeof(uint8_t), 1, File);
			size_t TestBitMapCount = fread(&m_BitMapCount, sizeof(uint32_t), 1, File);

			std::shared_ptr<uint64_t[]> m_BitMap(FY_NEW uint64_t[m_BitMapCount]);

			size_t TestBitMap = fread(&m_BitMap[0], sizeof(uint64_t), m_BitMapCount, File);

			m_PixWidth = m_CoordWidth * m_PixCoordRatio;
			m_PixHeight = m_CoordHeight * m_PixCoordRatio;
			m_FileName = m_MetaData.FileName;


			if (TestBitMap == m_BitMapCount)
			{
				returnValue = true;
			}
		}
		else
		{
			FY_ASSERT(0, "BoolMap File Not Open");
		}

		return returnValue;
	}

	bool BoolMap::DeleteFromMem()
	{


		m_BitMap.reset();
		m_BitMap = nullptr;

		return true;
	}

}
