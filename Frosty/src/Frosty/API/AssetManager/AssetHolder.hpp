#ifndef ASSETHOLDER_H
#define ASSETHOLDER_H
#include "ModelTemplate.hpp"

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

		ModelTemplate* GetModeltemplate(const uint16_t& AssetId);
		
		Luna::Material* GetMaterial(const uint16_t& AssetId);

	private:	//Functions






	};



}


#endif // !ASSETHOLDER_H

