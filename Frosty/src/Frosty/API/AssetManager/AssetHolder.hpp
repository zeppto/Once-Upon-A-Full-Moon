#ifndef ASSETHOLDER_H
#define ASSETHOLDER_H
#include "ModelTemplate.hpp"

namespace Frosty
{

	//struct AssetKey
	//{
	//	std::string m_AssetName;
	//	uint16_t m_AssetId;
	//};



	class AssetHolder
	{

	public:		//Variables

	private:	//Variables

		int32_t m_Container_Slot_Counter;

		std::unordered_map<int32_t, ModelTemplate> m_ModelMap;
		std::unordered_map<int32_t, Luna::Material> m_MaterialMap;

		//If we delete a object add the slot to this list
		std::list<int32_t> m_Deleted_Slot_List;

	public:		//Functions

		AssetHolder();
		virtual ~AssetHolder();

		const int32_t& GetEmptyContainerSlot() { return m_Container_Slot_Counter++; };

		ModelTemplate* GetModeltemplate(const int32_t& Container_Slot);
		Luna::Material* GetMaterial(const int32_t& Container_Slot);

	private:	//Functions

	};



}


#endif // !ASSETHOLDER_H

