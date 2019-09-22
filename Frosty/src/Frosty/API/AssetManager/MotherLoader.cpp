#include <fypch.hpp>
#include "MotherLoader.hpp"
#include "Assetmanager.hpp"



namespace Frosty
{

	MotherLoader* MotherLoader::m_Instance = nullptr;


	MotherLoader* MotherLoader::GetMotherLoader()
	{
		if (m_Instance == nullptr) 
		{
			m_Instance = new MotherLoader;
		}

		return m_Instance;
	}

	MotherLoader::~MotherLoader()
	{
		if (m_Instance != nullptr)
		{
			delete m_Instance;
		}
	}

	bool MotherLoader::Loadfile(std::string& FilePath, ModelTemplate& Mod, std::vector<Luna::Material>& Mats)
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

		//tempFile.readFile(FilePath.c_str()); //check if failed reading???
		tempFile.readFile(FilePath.c_str()); //check if failed reading???



		if (1) //read file check
		{



			Mod =  ModelTemplate();

			// for nr of meshes
			for (uint16_t i = 0; i < tempFile.getMeshCount(); i++)
			{
				Mod.GetMeshVector()->emplace_back(tempFile.getMesh(i));
				
				uint16_t tempMeshId = Mod.GetMeshVector()->back().id;

				ModelTemplate::MeshInfo* tempMeshInfo_Ptr = Mod.GetMeshInfo(tempMeshId);
					
				tempMeshInfo_Ptr->m_BoundingBox = tempFile.getBoundingBox(tempMeshId);
				
				tempFile.getIndices(tempMeshId, tempMeshInfo_Ptr->m_MeshIndices);
				tempFile.getWeights(tempMeshId, tempMeshInfo_Ptr->m_Weights);
				tempFile.getVertices(tempMeshId, tempMeshInfo_Ptr->m_MeshVertices);
				//Mod->getBoundingBoxVector()->emplace_back(tempFile.getBoundingBox(i));

			}


			//nr of models
			*Mod.GetAnimation() = tempFile.getAnimation();
			*Mod.GetSkeleton() = tempFile.getSkeleton();
	
			//vector fills
			tempFile.getJoints(*Mod.GetJointVector());

			for (uint16_t i = 0; i < Mod.GetJointVector()->size(); i++)
			{
				tempFile.getKeyframes(Mod.GetJointVector()->at(i).jointID, *Mod.GetKeyframes(Mod.GetJointVector()->at(i).jointID));
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

	bool MotherLoader::Loadfile(std::string& FilePath, bool Reload)
	{

		auto temp_AssetManager = Assetmanager::GetAssetmanager();

		std::string TempFileName = GetFileName(FilePath);
		if (!temp_AssetManager->FileLoaded(TempFileName))
		{
			ModelTemplate* mod_ptr;
			temp_AssetManager->AddNewModelTemplate(mod_ptr,TempFileName);


			//Fill modeltemplate











			//Check if material Exist and Fill



		}
		else
		{
			if (Reload)
			{


			}
			else
			{
				FY_CORE_INFO("File Already Loaded, File: {0}",TempFileName);
			}


		}



		return false;
	}

	std::string MotherLoader::GetFileName(const std::string& FilePath)
	{

		std::string temp_str = "";


		size_t count = (FilePath.size() - 1);
		while (FilePath[count] != (char)".")
		{
			count--;
		}

		count--;
		while (FilePath[count] != (char)"/")
		{
			temp_str.push_back(FilePath[count--]);
		}
		temp_str.reserve();
		return temp_str;
	}






}


