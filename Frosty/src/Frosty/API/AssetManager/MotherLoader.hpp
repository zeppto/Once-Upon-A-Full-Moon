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

		friend class Assetmanager;


	public: //functions
		static MotherLoader* GetMotherLoader();
		~MotherLoader();
		
		//bool Loadfile(const std::string& FilePath,const bool& Reload = false);
		bool Loadfile(const std::string& FilePath,const  std::string& PrefabName = "",const bool& Reload = false);
		void LoadFilesFromFile();


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
