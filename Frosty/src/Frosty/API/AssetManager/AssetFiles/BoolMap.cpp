#include<fypch.hpp>
#include"BoolMap.hpp"
#include<fstream>

//BoolMap::BoolMap(const uint16_t& Width, const uint16_t& Height, const uint8_t PixRatio, std::shared_ptr<bool[]>& Boolmap, std::shared_ptr<uint64_t[]> BitMap) : m_PixWidth(Width), m_PixHeight(Height),m_PixCoordRatio(PixRatio), m_BoolMap(Boolmap), m_CoordHeight(Height/ PixRatio), m_CoordWidth(Width / PixRatio) , m_BitMap(BitMap)
//{
//}
//

namespace Frosty
{
	int BoolMap::s_SavedMapCount = 0;

	BoolMap::BoolMap(const FileMetaData& metaData) :
		//	m_FileName(""),
		m_BitMap(0),
		m_PixWidth(0),
		m_PixHeight(0),
		m_PixCoordRatio(0),
		m_BitMapCount(0),
		m_CoordWidth(0),
		m_CoordHeight(0),
		AssetFile(metaData)
	{
	}
	BoolMap::BoolMap(const uint16_t& Width, const uint16_t& Height, const uint8_t PixRatio, std::shared_ptr<uint64_t[]> BitMap, const uint32_t& BitmapCount) :
		//	m_FileName(""),
		m_BitMap(BitMap),
		m_PixWidth(Width),
		m_PixHeight(Height),
		m_PixCoordRatio(PixRatio),
		m_BitMapCount(BitmapCount),
		m_CoordWidth(Width / PixRatio),
		m_CoordHeight(Height / PixRatio),
		AssetFile()
	{}

	BoolMap::BoolMap(const BoolMap& other) :
		m_BitMap(other.m_BitMap),
		//m_BoolMap(other.m_BoolMap), 
		m_PixWidth(other.m_PixWidth),
		m_PixHeight(other.m_PixHeight),
		m_BitMapCount(other.m_BitMapCount),
		m_CoordWidth(other.m_CoordWidth),
		m_CoordHeight(other.m_CoordHeight),
		m_PixCoordRatio(other.m_PixCoordRatio),
		AssetFile(other.m_MetaData)
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
			//m_FileName = other.m_FileName;
			m_CoordHeight = other.m_CoordHeight;
			m_CoordWidth = other.m_CoordWidth;
			m_BitMap = other.m_BitMap;
			m_PixCoordRatio = other.m_PixCoordRatio;
		}
		return *this;
	}

	bool BoolMap::CheckCollition(const glm::vec3& LocalPos) const
	{
		bool returnValue = false;
		if (m_CoordWidth >= LocalPos.x && 0 <= LocalPos.x && LocalPos.z >= 0 && LocalPos.z <= m_CoordHeight)
		{

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

	bool BoolMap::SaveMap(const std::string& FileName)
	{
		bool returnValue = false;
		if(m_CoordWidth && m_CoordHeight)
		{
			//File Name/Location
		std::string filePath;
		if (m_MetaData.FullFilePath == "")
		{
			m_MetaData.FileExtentionName = "bmap";
			if (FileName != "")
			{
				filePath = FileName;
			}
			else
			{
				filePath = "BoolMap_" + std::to_string(s_SavedMapCount++);
			}
			m_MetaData.FileName = filePath;
			filePath += "." + m_MetaData.FileExtentionName;
		}
		else
		{
			filePath = m_MetaData.FullFilePath;
		}



		//Saving the file

		std::FILE* File;
		File = fopen(filePath.c_str(), "w");

		if (File != nullptr)
		{



			FY_CORE_ASSERT(fwrite(&m_CoordWidth, sizeof(uint16_t), 1, File),"Could not write Width for Boolmap: {0}",filePath);
			FY_CORE_ASSERT(fwrite(&m_CoordHeight, sizeof(uint16_t), 1, File),"Could not write Heigth for Boolmap: {0}",filePath);
			FY_CORE_ASSERT(fwrite(&m_PixCoordRatio, sizeof(uint8_t), 1, File),"Could not write Pix/Coord ratio for Boolmap: {0}",filePath);
			FY_CORE_ASSERT(fwrite(&m_BitMapCount, sizeof(uint32_t), 1, File),"Could not write BitmapCount for Boolmap: {0}",filePath);

			size_t Test = fwrite(&m_BitMap[0], sizeof(uint64_t), m_BitMapCount, File);
			if (Test != m_BitMapCount)
			{
				FY_CORE_ASSERT(0, "Could not save Boolmap: {0}", filePath);
			}
			returnValue = true;
		}
		fclose(File);
	}
		else
		{
			FY_CORE_WARN("Cannot save a map with 0 values in width or heigth {0}", FileName);
		}

		return returnValue;
	}

	bool BoolMap::LoadMap(const std::string& FilePath)
	{

		bool returnValue = false;



		std::string filePath;

		if (FilePath != "")
		{
			filePath = FilePath;
		}
		else
		{
			filePath = m_MetaData.FullFilePath;
		}

		std::FILE* File;

		File = fopen(filePath.c_str(), "r");
		
		if (File != nullptr)
		{

			FY_CORE_ASSERT(fread(&m_CoordWidth, sizeof(uint16_t), 1, File), "Could not read Width for Boolmap: {0}", filePath);
			FY_CORE_ASSERT(fread(&m_CoordHeight, sizeof(uint16_t), 1, File), "Could not read Heigth for Boolmap: {0}", filePath);
			FY_CORE_ASSERT(fread(&m_PixCoordRatio, sizeof(uint8_t), 1, File), "Could not read Pix/Coord ratio for Boolmap: {0}", filePath);
			FY_CORE_ASSERT(fread(&m_BitMapCount, sizeof(uint32_t), 1, File), "Could not read BitmapCount for Boolmap: {0}", filePath);



			//	int bitSize = std::ceil((texSize / 64.0f));


			std::shared_ptr<uint64_t[]> bitMap(FY_NEW uint64_t[m_BitMapCount]);

			//Testing
			//size_t testCount = fread(&bitMap[0], sizeof(uint64_t), m_BitMapCount, File);
			size_t testCount = fread(&m_BitMap[0], sizeof(uint64_t), m_BitMapCount, File);
			if (testCount != m_BitMapCount)
			{
				FY_CORE_ASSERT(0, "Could not load Boolmap: {0}", filePath);
			}
			else
			{
				returnValue = true;
			}


			//Testing
			//for (uint32_t i = 0; i < m_BitMapCount; i++)
			//{
			//	if (bitMap[i] != m_BitMap[i])
			//	{
			//		int nein = 0;
			//	}
			//}

		}
		fclose(File);

		return returnValue;
	}
}


