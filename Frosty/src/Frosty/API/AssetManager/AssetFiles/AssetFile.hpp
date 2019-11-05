#ifndef ASSETFILE_HPP
#define ASSETFILE_HPP

namespace Frosty
{

	struct FileMetaData
	{
		int8_t Type = -1;
		std::string TagName = "";
		std::string FileName = "";
		std::string FullFilePath = "";
		std::string FileExtentionName = "";
	};


	class AssetFile
	{

	public:




	protected:

		FileMetaData m_MetaData;
		unsigned int m_GPU_Data_ID = -1;

		bool m_OnMem = false;
		bool m_OnGPU = false;

	public:
		inline AssetFile() : m_MetaData(FileMetaData()) {}
		inline AssetFile(const FileMetaData& MetaData) : m_MetaData(MetaData) {}
		inline  virtual ~AssetFile() {}

		inline void SetTagName(const std::string& PrefabName) { m_MetaData.TagName = PrefabName; }

		inline FileMetaData& GetfileMetaData() { return m_MetaData; };
		inline void SetFileMetaData(const FileMetaData& MetaData) { m_MetaData = MetaData; }

		virtual bool LoadToMem(const bool& Reload = false) = 0;
		virtual bool DeleteFromMem() = 0;

		virtual bool LoadToGPU() = 0;
		virtual bool DeleteFromGPU() = 0;

		inline const bool& LoadedOnCPU() const { return m_OnMem; }
		inline const bool& LoadedOnGPU() const { return m_OnGPU; }

		//not standard name (unsigned for openGL)
		inline const unsigned int& Get_GPU_Data_ID() const { return m_GPU_Data_ID; }


	private:


	};

}
#endif // !ASSETFILE_HPP
