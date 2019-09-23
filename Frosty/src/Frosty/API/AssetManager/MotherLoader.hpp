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


	public: //functions
		static MotherLoader* GetMotherLoader();
		~MotherLoader();
		
		bool Loadfile(std::string& FilePath, ModelTemplate& Mod, std::vector<Luna::Material>& Mats);
		bool Loadfile(std::string& FilePath,bool Reload = false);
		void LoadFilesFromFile();


	private: //functions



		std::string GetFileName(const std::string& FilePath);

	};



}


#endif // ! MOTHERLOADER_H
