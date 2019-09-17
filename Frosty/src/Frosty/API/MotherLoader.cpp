#include "MotherLoader.hpp"
#include <fypch.hpp>


namespace Frosty
{


	bool MotherLoader::Loadfile(std::string FilePath, ModelTemplate Mod, std::vector<Luna::Material>& Mats)
	{
		bool returnValue = false;

		//if (Mod != nullptr)
		//{
		//	delete Mod;
		//	Mod = nullptr;
		//}


		for (uint16_t i = 0; i < Mats.size(); i++)
		{
			Mats.erase(Mats.begin() + i);
		}


		Luna::Reader tempFile;

		tempFile.readFile(FilePath.c_str); //check if failed reading???

		if (1) //read file check
		{



			Mod =  ModelTemplate();

			// for nr of meshes
			for (uint16_t i = 0; i < tempFile.getMeshCount(); i++)
			{
				Mod.getMeshVector()->emplace_back(tempFile.getMesh(i));
				
				uint16_t tempMeshId = Mod.getMeshVector()->back().id;

				ModelTemplate::MeshInfo* tempMeshInfo_Ptr = Mod.getMeshInfo(tempMeshId);
					
				tempMeshInfo_Ptr->m_BoundingBox = tempFile.getBoundingBox(tempMeshId);
				
				tempFile.getIndices(tempMeshId, tempMeshInfo_Ptr->m_MeshIndices);
				tempFile.getWeights(tempMeshId, tempMeshInfo_Ptr->m_Weights);
				tempFile.getVertices(tempMeshId, tempMeshInfo_Ptr->m_MeshVertices);
				//Mod->getBoundingBoxVector()->emplace_back(tempFile.getBoundingBox(i));

			}


			//nr of models
			*Mod.getAnimation() = tempFile.getAnimation();
			*Mod.getSkeleton() = tempFile.getSkeleton();
	
			//vector fills
			tempFile.getJoints(*Mod.getJointVector());

			for (uint16_t i = 0; i < Mod.getJointVector()->size(); i++)
			{
				tempFile.getKeyframes(Mod.getJointVector()->at(i).jointID, *Mod.getKeyframes(Mod.getJointVector()->at(i).jointID));
			}




			//mats
			tempFile.getMaterials(Mats);

			returnValue = true;
		}
		else
		{
			FY_CORE_ERROR("Error Reading File: {0}", FilePath);
		}




		return returnValue;
	}






}


