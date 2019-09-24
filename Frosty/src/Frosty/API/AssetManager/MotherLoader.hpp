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
		static MotherLoader* m_Instance;

		bool m_AutoLoader = true;


	public: //functions
		static MotherLoader* GetMotherLoader();
		~MotherLoader();
		
		bool Loadfile(const std::string& FilePath,const bool& Reload = false);
		bool Loadfile(const std::string& FilePath,const  std::string& PrefabKey,bool Reload = false);
		void LoadFilesFromFile();


	private: //functions



		std::string GetFileName(const std::string& FilePath);

	};



}


#endif // ! MOTHERLOADER_H
