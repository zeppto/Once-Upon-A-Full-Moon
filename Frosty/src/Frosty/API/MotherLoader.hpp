#ifndef  MOTHERLOADER_H
#define MOTHERLOADER_H
#include<Luna/include/Luna.h> // temp
#include "ModelTemplate.hpp"
#include<fypch.hpp>
#include<Luna/include/Reader.h> // temp



namespace Frosty
{


	class MotherLoader
	{
	public: //variables




	private: //variables
		static MotherLoader* m_Instance;


	public: //functions
		static MotherLoader* GetMotherLoader();
		~MotherLoader();

		bool Loadfile(std::string FilePath, ModelTemplate Mod, std::vector<Luna::Material>& Mats);


	private: //functions





	};



}


#endif // ! MOTHERLOADER_H
