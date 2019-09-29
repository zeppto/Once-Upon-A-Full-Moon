#ifndef  MOTHERLOADER_H
#define MOTHERLOADER_H
#include "ModelTemplate.hpp"
#include<Luna/include/Reader.h>



// should be inside definitions?
//File types
#define FILE_TYPE_JPG "jpg"
#define FILE_TYPE_PNG "png"
#define FILE_TYPE_LUNA "lu"



namespace Frosty
{

	extern 	struct FileMetaData;



	enum FileType
	{
		JPG,
		PNG,
		LUNA
	};

	struct TextureFile
	{

		//std::shared_ptr<unsigned char> Image_Data_Ptr;
		bool Loaded_In_Gpu = false;
		//Open gl needed an unsigned int
		unsigned int  Buffer_ID;
		uint16_t Image_Width;
		uint16_t Image_Height;

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

		bool LoadGraphicFileToGPU(unsigned char* Texture_Data, TextureFile& Texture_File);

	};



}


#endif // ! MOTHERLOADER_H
