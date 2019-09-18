#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H
#include<fypch.hpp>
#include"MotherLoader.hpp"
#include"AssetHolder.hpp"

namespace Frosty
{

	

	class Assetmanager {


	public:		//Variables

	private:	//Variables
		static Assetmanager* m_Instance;
		AssetHolder m_AssetHolder;

	public:		//Functions

		static Assetmanager* GetAssetmanager();
		~Assetmanager();
		
		
		bool LoadFile(std::string FilePath);
		bool LoadFile(std::string FilePath, const std::string& set_Prefabkey);



	private:	//Functions


	};

}

#endif // !ASSETMANAGER_H

