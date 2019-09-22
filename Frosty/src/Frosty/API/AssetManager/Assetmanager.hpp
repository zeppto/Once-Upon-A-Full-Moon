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

		std::vector<std::string> m_Model_FileName_Vector; // same as material
		std::vector<std::string> m_Material_FileName_Vector; 
	

	public:		//Functions

		static Assetmanager* GetAssetmanager();
		~Assetmanager();
		
		
		bool LoadFile(std::string FilePath);
		//Not initialized yet
		bool LoadFile(std::string FilePath, const std::string& set_Label);

		void AddNewModelTemplate(ModelTemplate* ModelTemplate,const std::string& FileName);
		void AddNewMaterialTemplate(ModelTemplate* ModelTemplate,const std::string& FileName);

		AssetMetaData<ModelTemplate> const* GetModeltemplateMetaData(const std::string& FileName);
		AssetMetaData<Luna::Material> const* GetMaterialMetaData(const std::string& FileName);

		ModelTemplate* GetModelTemplate();

		bool FileLoaded(std::string FileName);

	private:	//Functions
		bool CheckIfMetaDataExist(std::string FileName);


	};

}

#endif // !ASSETMANAGER_H

