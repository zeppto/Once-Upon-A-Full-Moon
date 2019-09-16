#ifndef  MOTHERLOADER_H
#define MOTHERLOADER_H
#include<Luna.h>
#include<fypch.hpp>
#include<Reader.h>


namespace Frosty
{


	class MotherLoader
	{
	public: //variables




	private: //variables



	public: //functions
		MotherLoader();
		~MotherLoader();

		bool Loadfile(std::string FilePath, Luna::Model* Mod, std::vector<Luna::Material*>& Mats);


	private: //functions





	};



}


#endif // ! MOTHERLOADER_H
