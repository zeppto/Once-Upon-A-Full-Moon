#ifndef  MOTHERLOADER_H
#define MOTHERLOADER_H
#include"AssetFiles/ModelTemplate.hpp"
#include<Luna/include/Reader.h>
#include"AssetFiles/TextureFile.hpp"

// should be inside definitions?
//File types
#define FILE_TYPE_JPG "jpg"
#define FILE_TYPE_PNG "png"
#define FILE_TYPE_LUNA "lu"
#define FILE_TYPE_TTF "ttf"

namespace Frosty
{
	extern 	struct FileMetaData;

	enum FileType
	{
		JPG,
		PNG,
		LUNA,
		TTFF
	};
	
	class MotherLoader
	{
	public: //variables



	private: //variables


		static MotherLoader* s_Instance;

		static uint16_t s_Failed_Loading_Attempts;
		static uint16_t s_Success_Loading_Attempts;

		friend class Assetmanager;


	public: //functions
		static MotherLoader* GetMotherLoader();
		~MotherLoader();

		//bool Loadfile(const std::string& FilePath,const bool& Reload = false);
		bool Loadfile(const std::string& FilePath, const  std::string& PrefabName = "", const bool& Reload = false);
		void LoadFiles();

		const uint16_t& GetNrOfFailedAttempts() const { return s_Failed_Loading_Attempts; }
		const uint16_t& GetNrOfSuccessAttempts() const { return s_Success_Loading_Attempts; }
		const uint16_t GetNrOfTotalAttempts() const { return s_Success_Loading_Attempts + s_Failed_Loading_Attempts; }

		void PrintLoadingAttemptInformation() const;




	private: //functions


		inline static void Delete() { if (s_Instance != nullptr) { delete s_Instance; } }
		bool GetFileInformation(FileMetaData& FileNameInformation);
		int8_t GetFileType(const std::string& fileType) const;

		//AssetManager Filler Functions
		bool LoadLunaFile(const FileMetaData& FileNameInformation, const bool& Reload = false);
		bool LoadGraphicFile(const FileMetaData& FileNameInformation, const bool& Reload = false);
	

	};
}
#endif 
