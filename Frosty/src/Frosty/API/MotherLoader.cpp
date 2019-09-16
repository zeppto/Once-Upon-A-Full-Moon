#include "MotherLoader.hpp"
#include <fypch.hpp>

namespace Frosty
{


	bool MotherLoader::Loadfile(std::string FilePath, Luna::Model* Mod, std::vector<Luna::Material*>& Mats)
	{
		bool returnValue = false;

		if (Mod != nullptr)
		{
			delete Mod;
			Mod = nullptr;
		}


		for (uint16_t i = 0; i < Mats.size(); i++)
		{
			Mats.erase(Mats.begin() + i);
		}






		Luna::Reader tempFile;

		tempFile.readFile(FilePath.c_str); //check if failed reading???

		if (1) //read file check
		{
			// is this correct?
			for (uint16_t i = 0; i < tempFile.getMaterialCount(); i++)
			{
				Mats.emplace_back(tempFile.getMaterial(i));
			}


			Mod = new Luna::Model;


			for (uint16_t i = 0; i < tempFile.getMeshCount(); i++)
			{
				Mod->meshes.emplace_back(tempFile.getMesh(i));
			}


		}
		else
		{
			FY_CORE_ERROR("Error Reading File: {0}", FilePath);
		}




		return returnValue;
	}






}


