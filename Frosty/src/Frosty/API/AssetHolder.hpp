#ifndef ASSETHOLDER_H
#define ASSETHOLDER_H
#include "fypch.hpp"
#include<Luna.h>

namespace Frosty
{

	struct AssetKey
	{

		std::string m_AssetName;
		uint16_t m_AssetId;


	};




	class AssetHolder
	{


	public:		//Variables

	private:	//Variables

		std::unordered_map<int, Luna::Model> m_ModelMap;
		std::unordered_map<int, Luna::Material> m_MaterialMap;



	public:		//Functions

		AssetHolder();
		~AssetHolder();

	private:	//Functions






	};



}


#endif // !ASSETHOLDER_H

