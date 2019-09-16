#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H
#include<fypch.hpp>
#include"MotherLoader.hpp"

namespace Frosty
{
	
	

	class Assetmanager {


	public:		//Variables

	private:	//Variables
		static Assetmanager* m_Instance;

	public:		//Functions

		static Assetmanager* getAssetmanager();

		


	private:	//Functions

		~Assetmanager();

	};

}

#endif // !ASSETMANAGER_H

