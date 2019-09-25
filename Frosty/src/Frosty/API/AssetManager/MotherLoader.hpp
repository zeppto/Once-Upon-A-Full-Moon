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
			int8_t m_type = -1;
			std::string m_FilePath = "";
			std::string m_FileName = "";
			std::string m_PreFab_Name = "";
		};

		static MotherLoader* s_Instance;

		uint16_t m_Total_Loading_Attempts;
		uint16_t m_Failed_Loading_Attempts;
		uint16_t m_Success_Loading_Attempts;

		friend class Assetmanager;


	public: //functions
		static MotherLoader* GetMotherLoader();
		~MotherLoader();
		
		//bool Loadfile(const std::string& FilePath,const bool& Reload = false);
		bool Loadfile(const std::string& FilePath,const  std::string& PrefabName = "",const bool& Reload = false);
		void LoadFiles();

		const uint16_t& GetNrOfFailedAttempts() const { return m_Failed_Loading_Attempts; }
		const uint16_t& GetNrOfSuccessAttempts() const { return m_Success_Loading_Attempts; }
		const uint16_t& GetNrOfTotalAttempts() const { return m_Total_Loading_Attempts; }

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
