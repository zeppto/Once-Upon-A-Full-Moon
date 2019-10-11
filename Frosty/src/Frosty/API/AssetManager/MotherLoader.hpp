#ifndef  MOTHERLOADER_H
#define MOTHERLOADER_H
#include"AssetFiles/ModelTemplate.hpp"
#include<Luna/include/Reader.h>
#include"AssetFiles/TextureFile.hpp"
#include "AssetFiles/TrueTypeFile.hpp"

//or enums?

// should be inside definitions?
//File types
#define FILE_TYPE_JPG "jpg"
#define FILE_TYPE_PNG "png"
#define FILE_TYPE_LUNA "lu"
#define FILE_TYPE_TTF "ttf"
//#define FILE_TYPE_PARTICLE "particle" //For exported particle systems (not yet implemented)

namespace Frosty
{

	extern 	struct FileMetaData;

	enum FileType
	{
		JPG,
		PNG,
		LUNA,
		TTF
		//PARTICLE
	};

	class MotherLoader
	{
	public: //variables

	private: //variables

		enum FileType
		{
			JPG,
			PNG,
			LUNA,
			TTF
		};
		struct FileNameInfo
		{
			int8_t m_type = -1;
			std::string m_FilePath = "";
			std::string m_FileName = "";
			std::string m_PreFab_Name = "";
		};

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

		bool LoadFontFile(const FileMetaData& FileNameInformation, const bool& Reload = false);
		bool LoadLunaFile(const FileMetaData& FileNameInformation, const bool& Reload = false);
		bool LoadGraphicFile(const FileMetaData& FileNameInformation, const bool& Reload = false);
		//bool LoadParticleFile(const FileMetaData& FileNameInformation, const bool& Reload = false);
	};
}


#endif // ! MOTHERLOADER_H
