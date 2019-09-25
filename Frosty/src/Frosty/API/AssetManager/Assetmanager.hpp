#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include"MotherLoader.hpp"
#include"AssetHolder.hpp"
#include"KeyLabel.hpp"



namespace Frosty
{

	

	class Assetmanager {


	public:		//Variables

	private:	//Variables
		static Assetmanager* s_Instance;
		AssetHolder m_AssetHolder;

		uint16_t m_Total_Nr_Assets;




	
		std::unordered_map<std::string, AssetMetaData<ModelTemplate>>  m_MT_MetaData_Map;
		std::unordered_map<std::string, AssetMetaData<Luna::Material>>  m_MAT_MetaData_Map;

		//may not be needed, string information are also stored in the unordered maps above
		std::vector<std::string> m_FilePath_Vector; 
		std::vector<std::string> m_Material_File_Name_Vector; // Diffuse File Path
		std::vector<std::string> m_ModelTemplate_File_Name_Vector; // Same as file name for now
	
		friend class Application;

	public:		//Functions

		static Assetmanager* GetAssetmanager();
		~Assetmanager();
		
		
		bool LoadFile(const std::string& FilePath, const std::string& PreFab_Name = "");
		//Not initialized yet
	//	bool LoadFile(std::string FilePath, const std::string& set_Label);

		bool AddNewModelTemplate(ModelTemplate*& ModelTemplate,const std::string& FileName, const std::string& FilePath);
		bool AddNewMaterialTemplate(Luna::Material*& Material,const std::string& FileName, const std::string& FilePath);

		/* if needed insert reload functions here
		.
		.
		.
		.
		*/

		AssetMetaData<ModelTemplate> * GetModeltemplateMetaData(const std::string& FileName);
		AssetMetaData<Luna::Material> * GetMaterialMetaData(const std::string& FileName);


		//KeyLabel<ModelTemplate>& CreateModelKey(std::string AssetName);
		//KeyLabel<Luna::Material>& CreateMaterialKey(std::string AssetName);

		bool LinkModelKey(const std::string& FileName, KeyLabel<ModelTemplate>* In_Key);
		bool LinkMaterialKey(const std::string& FileName, KeyLabel<Luna::Material>* In_Key);

		bool LinkKey(const std::string& FileName, BaseKey* In_Key);

	private:	//Functions
		
		bool FileLoaded(const std::string& FilePath);
		bool AssetLoaded(const std::string& FileName);

		bool MaterialLoaded(const std::string& FileName);
		bool ModelTemplateLoaded(const std::string& FileName);
		
		bool CheckIfMetaDataExist(const std::string& FilePath);


		inline static void Delete() { if (s_Instance != nullptr) { delete s_Instance; } }

	};

}

#endif // !ASSETMANAGER_H

