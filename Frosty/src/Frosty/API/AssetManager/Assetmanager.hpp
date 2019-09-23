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
		static Assetmanager* m_Instance;
		AssetHolder m_AssetHolder;

		uint16_t m_NrOfMaterial;
	
		std::unordered_map<std::string, AssetMetaData<ModelTemplate>>  m_MT_MetaData_Map;
		std::unordered_map<std::string, AssetMetaData<Luna::Material>>  m_MAT_MetaData_Map;

		//Not really needed, string information are also stored in the unordered maps above
		std::vector<std::string> m_FileName_Vector; 
		std::vector<std::string> m_Material_Asset_Name_Vector; // Diffuse File Path
		std::vector<std::string> m_ModelTemplate_Asset_Name_Vector; // Same as file name for now
	

	public:		//Functions

		static Assetmanager* GetAssetmanager();
		~Assetmanager();
		
		
		bool LoadFile(std::string FilePath);
		//Not initialized yet
		bool LoadFile(std::string FilePath, const std::string& set_Label);

		bool AddNewModelTemplate(ModelTemplate*& ModelTemplate,const std::string& AssetName, const std::string& FileName);
		bool AddNewMaterialTemplate(Luna::Material*& Material,const std::string& AssetName, const std::string& FileName);

		/*insert reload functions if needed here
		.
		.
		.
		.
		*/

		AssetMetaData<ModelTemplate> * GetModeltemplateMetaData(const std::string& AssetName);
		AssetMetaData<Luna::Material> const* GetMaterialMetaData(const std::string& AssetName);


		//KeyLabel<ModelTemplate>& CreateModelKey(std::string AssetName);
		//KeyLabel<Luna::Material>& CreateMaterialKey(std::string AssetName);

		bool LinkModelKey(const std::string& AssetName, KeyLabel<AssetMetaData<ModelTemplate>>* In_Key);
		bool LinkMaterialKey(const std::string& AssetName, KeyLabel<AssetMetaData<Luna::Material>>* In_Key);


	private:	//Functions
		
		bool FileLoaded(const std::string& FileName);
		bool AssetLoaded(const std::string& AssetName);

		bool MaterialLoaded(const std::string& AssetName);
		bool ModelTemplateLoaded(const std::string& AssetName);
		
		bool CheckIfMetaDataExist(std::string FileName);


	};

}

#endif // !ASSETMANAGER_H

