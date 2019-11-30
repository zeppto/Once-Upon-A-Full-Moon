#ifndef BOOLMAP
#define BOOLMAP
#include "AssetFile.hpp"


//File info
/*
1. Width
2. Heigth
3. Pix/Coord ratio
4. BitmapCount
5. BitMap
*/



namespace Frosty {

	class BoolMap : public AssetFile
	{
	private:

		uint16_t m_PixWidth;
		uint16_t m_PixHeight;

		uint16_t m_CoordHeight;
		uint16_t m_CoordWidth;

		uint8_t m_PixCoordRatio;
		//May not be needed
		uint32_t m_BitMapCount;

		//std::string m_FileName;

		//	std::shared_ptr<bool[]> m_BoolMap;
		std::shared_ptr<uint64_t[]> m_BitMap;

		static int s_SavedMapCount;

	public:

		inline BoolMap() = delete;
		BoolMap(const FileMetaData& metaData);
		//BoolMap(const uint16_t& Width, const uint16_t& Height, const uint8_t PixRatio,std::shared_ptr<bool[]>& Boolmap, std::shared_ptr<uint64_t[]> BitMap);
		BoolMap(const uint16_t& Width, const uint16_t& Height, const uint8_t PixRatio, std::shared_ptr<uint64_t[]> BitMap, const uint32_t& BitmapCount);
		BoolMap(const BoolMap& other);

		BoolMap& operator= (const BoolMap& other);

		bool SaveMap(const std::string& FileName = "");
		bool LoadMap(const std::string& FilePath);

		bool CheckCollision(const glm::vec3& LocalPos) const;

		inline const uint16_t& GetPixWidth() { return m_PixWidth; }
		inline const uint16_t& GetPixHeight() { return m_PixHeight; }
		inline const uint16_t& GetCoordWidth() { return m_CoordWidth; }
		inline const uint16_t& GetCoordHeight() { return m_CoordHeight; }
		inline const uint16_t& GetPixCoordRatio() { return m_PixCoordRatio; }
		//inline const std::shared_ptr<bool[]>& GetBoolMap() { return m_BoolMap; }


	};


}

#endif // !BOOLMAP
