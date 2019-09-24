#ifndef  MOTHERLOADER_H
#define MOTHERLOADER_H
#include "ModelTemplate.hpp"
#include<Luna/include/Reader.h>



namespace Frosty
{


	class MotherLoader
	{
	public: //variables



	private: //variables
		static MotherLoader* s_Instance;

		bool m_AutoLoader = true;

		friend class Assetmanager;


	public: //functions
		static MotherLoader* GetMotherLoader();
		~MotherLoader();
		
		bool Loadfile(const std::string& FilePath,const bool& Reload = false);
		bool Loadfile(const std::string& FilePath,const  std::string& PrefabKey,bool Reload = false);
		void LoadFilesFromFile();


	private: //functions


		inline static void Delete() {if (s_Instance != nullptr)	{delete s_Instance;}}
		std::string GetFileName(const std::string& FilePath);

	};



}


#endif // ! MOTHERLOADER_H
