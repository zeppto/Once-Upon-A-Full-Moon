#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include"MotherLoader.hpp"
#include"KeyLabel.hpp"





namespace Frosty
{

	struct LinkedMaterial
	{
		Luna::Material LunaMaterial;

		std::string Glow_File_Name;
		std::string Normal_File_Name;
		std::string Diffuse_File_Name;

		Frosty::AssetMetaData<Frosty::TextureFile>* Glow_Texture_MetaData_Ptr;
		Frosty::AssetMetaData<Frosty::TextureFile>* Normal_Texture_MetaData_Ptr;
		Frosty::AssetMetaData<Frosty::TextureFile>* Diffuse_Texture_MetaData_Ptr;

		LinkedMaterial() {

			std::string Glow_File_Name = "";
			std::string Normal_File_Name = "";
			std::string Diffuse_File_Name = "";

			Glow_Texture_MetaData_Ptr = nullptr;
			Normal_Texture_MetaData_Ptr = nullptr;
			Diffuse_Texture_MetaData_Ptr = nullptr;

		};
		LinkedMaterial(const Luna::Material& LunaMat) : LunaMaterial(LunaMat) {
			std::string Glow_File_Name = "";
			std::string Normal_File_Name = "";
			std::string Diffuse_File_Name = "";

			Glow_Texture_MetaData_Ptr = nullptr;
			Normal_Texture_MetaData_Ptr = nullptr;
			Diffuse_Texture_MetaData_Ptr = nullptr;
		};

	};


	

	class Assetmanager {


	public:		//Variables

	private:	//Variables
		static Assetmanager* s_Instance;
		uint16_t m_Total_Nr_Assets;

		std::unordered_map<std::string, AssetMetaData<ModelTemplate>>  m_MT_MetaData_Map;
		std::unordered_map<std::string, AssetMetaData<LinkedMaterial>>  m_MAT_MetaData_Map;
		std::unordered_map<std::string, AssetMetaData<TextureFile>>  m_Texture_MetaData_Map;

		//may not be needed, string information are also stored in the unordered maps above
		std::vector<std::string> m_FilePath_Vector; 
		std::vector<std::string> m_Texture_File_Name_Vector;
		std::vector<std::string> m_Material_File_Name_Vector; // Diffuse File Path
		std::vector<std::string> m_ModelTemplate_File_Name_Vector; // Same as file name for now
	
		friend class Application;

	public:		//Functions

		static Assetmanager* GetAssetmanager();
		~Assetmanager();
		
		
		bool LoadFile(const std::string& FilePath, const std::string& PreFab_Name = "");
		//Not initialized yet
	//	bool LoadFile(std::string FilePath, const std::string& set_Label);

		//Can be combined: (Later task if needed)

		bool AddNewModelTemplate(std::shared_ptr<ModelTemplate>& ModelTemplate, const FileMetaData& MetaData);
		bool AddNewTextureTemplate(const TextureFile& Texture,const FileMetaData& MetaData);
		//bool AddNewTextureTemplate(const FileMetaData& MetaData);
		bool AddNewMaterialTemplate(const Luna::Material& Material, const FileMetaData& MetaData);

		std::shared_ptr<TextureFile> AddNewTextureTemplate(const FileMetaData& MetaData);
		/* if needed insert reload functions here
		.
		.
		.
		.
		*/

		AssetMetaData<ModelTemplate> * GetModeltemplateMetaData(const std::string& FileName);
		AssetMetaData<LinkedMaterial> * GetMaterialMetaData(const std::string& FileName);
		AssetMetaData<TextureFile> * GetTextureMetaData(const std::string& FileName);


		//KeyLabel<ModelTemplate>& CreateModelKey(std::string AssetName);
		//KeyLabel<Luna::Material>& CreateMaterialKey(std::string AssetName);

		bool LinkModelKey(const std::string& FileName, KeyLabel<ModelTemplate>* In_Key);
		bool LinkMaterialKey(const std::string& FileName, KeyLabel<LinkedMaterial>* In_Key);

		bool LinkKey(const std::string& FileName, BaseKey* In_Key);

	private:	//Functions
		
		bool ConnectMaterialWithTexture(LinkedMaterial& Material, const FileMetaData& MetaData);

		bool FileLoaded(const std::string& FilePath);
		bool AssetLoaded(const std::string& FileName);

		bool MaterialLoaded(const std::string& FileName);
		bool ModelTemplateLoaded(const std::string& FileName);
		bool TextureLoaded(const std::string& FileName);

		unsigned int LoadGraphicFileToGPU(const std::string& FilePath);
		unsigned int LoadModelToGPU(ModelTemplate& ModelTemplate);
		
		//bool CheckIfMetaDataExist(const std::string& FilePath);

		const std::string CutFileExtentionFromString(const char* in_char_ptr);
		//const std::string CharToTring(const char* in_char_ptr);
		

		inline static void Delete() { if (s_Instance != nullptr) { delete s_Instance; } }

	};

}

#endif // !ASSETMANAGER_H

