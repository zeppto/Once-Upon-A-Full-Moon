#ifndef  MOTHERLOADER_H
#define MOTHERLOADER_H
#include "ModelTemplate.hpp"
#include<Luna/include/Reader.h>

//or enums?
// should be inside definitions?
//File types
#define FILE_TYPE_JPG "jpg"
#define FILE_TYPE_PNG "png"
#define FILE_TYPE_LUNA "lu"
//#define STBI_NO_PNG


namespace Frosty
{


	class MotherLoader
	{
	public: //variables



	private: //variables

		enum FileType
		{
			JPG,
			PNG,
			LUNA
		};
		struct FileNameInfo
		{
			int8_t Type = -1;
			std::string FilePath = "";
			std::string FileName = "";
			std::string PreFab_Name = "";
		};
		struct ImageFileInformation
		{

			uint16_t Image_Width = 0;
			uint16_t Image_Height = 0;

		};

		static MotherLoader* s_Instance;

		static uint16_t s_Failed_Loading_Attempts;
		static uint16_t s_Success_Loading_Attempts;

		friend class Assetmanager;


	public: //functions
		static MotherLoader* GetMotherLoader();
		~MotherLoader();
		
		//bool Loadfile(const std::string& FilePath,const bool& Reload = false);
		bool Loadfile(const std::string& FilePath,const  std::string& PrefabName = "",const bool& Reload = false);
		void LoadFiles();

		const uint16_t& GetNrOfFailedAttempts() const { return s_Failed_Loading_Attempts; }
		const uint16_t& GetNrOfSuccessAttempts() const { return s_Success_Loading_Attempts; }
		const uint16_t GetNrOfTotalAttempts() const { return s_Success_Loading_Attempts + s_Failed_Loading_Attempts; }

		void PrintLoadingAttemptInformation() const;


	private: //functions


		inline static void Delete() {if (s_Instance != nullptr)	{delete s_Instance;}}
		bool GetFileInformation(FileNameInfo& FileNameInformation);
		int8_t GetFileType(const std::string& fileType) const;

		//AssetManager Filler Functions
		bool LoadLunaFile(const FileNameInfo& FileNameInformation, const bool& Reload = false);
		bool LoadGraphicFile(const FileNameInfo& FileNameInformation, const bool& Reload = false);

	};



}


#endif // ! MOTHERLOADER_H
