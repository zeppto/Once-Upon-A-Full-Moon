#ifndef ASSETHOLDER_H
#define ASSETHOLDER_H
#include "fypch.hpp"
#include "ModelTemplate.hpp"
#include <Luna/include/Luna.h> // temp

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

		std::unordered_map<uint16_t, ModelTemplate> m_ModelMap;
		std::unordered_map<uint16_t, Luna::Material> m_MaterialMap;

	public:		//Functions

		AssetHolder();
		virtual ~AssetHolder();

		ModelTemplate* GetModeltemplate(uint16_t key);
		Luna::Material* GetMaterial(uint16_t key);

	private:	//Functions






	};



}


#endif // !ASSETHOLDER_H

