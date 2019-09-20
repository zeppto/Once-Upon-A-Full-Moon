#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include"MotherLoader.hpp"
#include"AssetHolder.hpp"

namespace Frosty
{

	

	class Assetmanager {


	public:		//Variables

	private:	//Variables
		static Assetmanager* m_Instance;
		AssetHolder m_AssetHolder;

		uint16_t m_NrOfMaterial;

	public:		//Functions

		static Assetmanager* GetAssetmanager();
		~Assetmanager();
		
		
		bool LoadFile(std::string FilePath);
		//Not initialized yet
		bool LoadFile(std::string FilePath, const std::string& set_Prefabkey);

		ModelTemplate* GetModelTemplate();

	private:	//Functions


	};

}

#endif // !ASSETMANAGER_H

