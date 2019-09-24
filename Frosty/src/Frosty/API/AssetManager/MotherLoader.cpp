#include <fypch.hpp>
#include "MotherLoader.hpp"
#include "Assetmanager.hpp"
#include "..\PrefabManager\PrefabManager.h"


namespace Frosty
{

	MotherLoader* MotherLoader::s_Instance = nullptr;


	MotherLoader* MotherLoader::GetMotherLoader()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = new MotherLoader;
		}

		return s_Instance;
	}

	MotherLoader::~MotherLoader()
	{
		//if (s_Instance != nullptr)
		//{
		//	delete s_Instance;
		//}
	}


	bool MotherLoader::Loadfile(const std::string& FilePath, const bool& Reload)
	{
		bool returnValue = false;


		auto temp_AssetManager = Assetmanager::GetAssetmanager();

		ModelTemplate* mod_ptr = nullptr;

		std::string TempFileName = GetFileName(FilePath);

		Luna::Reader tempFile;

		if (tempFile.readFile(FilePath.c_str())) //check if loading went well
		{

			std::string Temp_MT_Asset_Name = TempFileName; //ModelName?

			//ModelTemplate
			if (temp_AssetManager->AddNewModelTemplate(mod_ptr, Temp_MT_Asset_Name, TempFileName))
			{
				//Fill modeltemplate
				returnValue = true;
				// for nr of meshes
				for (uint16_t i = 0; i < tempFile.getMeshCount(); i++)
				{
					mod_ptr->GetMeshVector()->emplace_back(tempFile.getMesh(i));

					uint16_t tempMeshId = mod_ptr->GetMeshVector()->back().id;

					ModelTemplate::MeshInfo* tempMeshInfo_Ptr = mod_ptr->GetMeshInfo(tempMeshId);

					tempMeshInfo_Ptr->m_BoundingBox = tempFile.getBoundingBox(tempMeshId);

					tempFile.getIndices(tempMeshId, tempMeshInfo_Ptr->m_MeshIndices);
					tempFile.getWeights(tempMeshId, tempMeshInfo_Ptr->m_Weights);
					tempFile.getVertices(tempMeshId, tempMeshInfo_Ptr->m_MeshVertices);
					//Mod->getBoundingBoxVector()->emplace_back(tempFile.getBoundingBox(i));

				}

				//nr of models
				*mod_ptr->GetAnimation() = tempFile.getAnimation();
				*mod_ptr->GetSkeleton() = tempFile.getSkeleton();

				//vector fills
				tempFile.getJoints(*mod_ptr->GetJointVector());

				for (uint16_t i = 0; i < mod_ptr->GetJointVector()->size(); i++)
				{
					tempFile.getKeyframes(mod_ptr->GetJointVector()->at(i).jointID, *mod_ptr->GetKeyframes(mod_ptr->GetJointVector()->at(i).jointID));
				}


			}
			else
			{
				if (Reload)
				{

					FY_CORE_INFO("Trying To Reload a ModelTemplate: {0}", TempFileName);


				}
				else
				{
					FY_CORE_INFO("ModelTemplate Already Loaded, File: {0}", TempFileName);
				}


			}



			//Get Material Names
			std::vector<Luna::Material> tempMatVector;
			tempFile.getMaterials(tempMatVector);

			Luna::Material* tempMatPtr = nullptr;

			//Add Materials to holder
			for (int i = 0; i < tempMatVector.size(); i++)
			{

				//tempMatVector.at(i).diffuseTexPath Chop Name???

				//Materials
				if (temp_AssetManager->AddNewMaterialTemplate(tempMatPtr, tempMatVector.at(i).diffuseTexPath, TempFileName))
				{
					//Fill Material
					*tempMatPtr = tempMatVector.at(i);
				}
				else
				{
					if (Reload)
					{
						FY_CORE_INFO("Trying To Reload a Material: {0}", TempFileName);
					}
					else
					{
						FY_CORE_INFO("Material Already Loaded, File: {0}", TempFileName);
					}
				}
			}
		}
		else
		{
			FY_CORE_WARN("Failed To Load File, File: {0}", TempFileName);
		}


		return returnValue;
	}

	bool MotherLoader::Loadfile(const std::string& FilePath, const std::string& PrefabKey, bool Reload)
	{


		bool returnValue = false;


		auto temp_AssetManager = Assetmanager::GetAssetmanager();

		ModelTemplate* mod_ptr = nullptr;


		Luna::Reader tempFile;
		std::string TempFileName = GetFileName(FilePath);


		if (tempFile.readFile(FilePath.c_str())) //check if loading went well
		{


			std::string Temp_MT_Asset_Name = TempFileName; //ModelName?

			//ModelTemplate
			if (temp_AssetManager->AddNewModelTemplate(mod_ptr, Temp_MT_Asset_Name, TempFileName))
			{
				//Fill modeltemplate

				returnValue = true;
				bool modelHasSkeleton = false;

				// for nr of meshes
				for (uint16_t i = 0; i < tempFile.getMeshCount(); i++)
				{
					mod_ptr->GetMeshVector()->emplace_back(tempFile.getMesh(i));

					uint16_t tempMeshId = mod_ptr->GetMeshVector()->back().id;

					ModelTemplate::MeshInfo* tempMeshInfo_Ptr = mod_ptr->GetMeshInfo(tempMeshId);

					if (mod_ptr->GetMeshVector()->back().hasSkeleton)
					{
						tempFile.getWeights(tempMeshId, tempMeshInfo_Ptr->m_Weights);
						modelHasSkeleton = true;
					}

					if (mod_ptr->GetMeshVector()->back().hasBoundingBox)
					{
						tempMeshInfo_Ptr->m_BoundingBox = tempFile.getBoundingBox(tempMeshId);
					}

					tempFile.getIndices(tempMeshId, tempMeshInfo_Ptr->m_MeshIndices);
					tempFile.getVertices(tempMeshId, tempMeshInfo_Ptr->m_MeshVertices);

					//Mod->getBoundingBoxVector()->emplace_back(tempFile.getBoundingBox(i));

				}

				if (modelHasSkeleton)
				{
					*mod_ptr->GetSkeleton() = tempFile.getSkeleton();

				}

				if (tempFile.animationExist())
				{
					*mod_ptr->GetAnimation() = tempFile.getAnimation();

				}

				//nr of models


				//vector fills
				tempFile.getJoints(*mod_ptr->GetJointVector());

				for (uint16_t i = 0; i < mod_ptr->GetJointVector()->size(); i++)
				{
					tempFile.getKeyframes(mod_ptr->GetJointVector()->at(i).jointID, *mod_ptr->GetKeyframes(mod_ptr->GetJointVector()->at(i).jointID));
				}


			}
			else
			{
				if (Reload)
				{

					FY_CORE_INFO("Trying To Reload a ModelTemplate: {0}", TempFileName);


				}
				else
				{
					FY_CORE_INFO("ModelTemplate Already Loaded, File: {0}", TempFileName);
				}


			}



			//Get Material Names
			std::vector<Luna::Material> tempMatVector;
			tempFile.getMaterials(tempMatVector);

			Luna::Material* tempMatPtr = nullptr;



			std::string MaterialAssetName = "HejHej Fel Fel";


			//Add Materials to holder
			for (int i = 0; i < tempMatVector.size(); i++)
			{

				//tempMatVector.at(i).diffuseTexPath Chop Name???

				//Materials
				if (temp_AssetManager->AddNewMaterialTemplate(tempMatPtr, tempMatVector.at(i).diffuseTexPath, TempFileName))
				{
					//Fill Material
					*tempMatPtr = tempMatVector.at(i);

					//saving lates material name for prefab(If Needed, build so the prefab can support more materials)
					MaterialAssetName = tempMatVector.at(i).diffuseTexPath;

				}
				else
				{
					if (Reload)
					{
						FY_CORE_INFO("Trying To Reload a Material: {0}", TempFileName);
					}
					else
					{
						FY_CORE_INFO("Material Already Loaded, File: {0}", TempFileName);
					}
				}
			}



			PrefabManager::GetPrefabManager()->setPrefab(PrefabKey, TempFileName, MaterialAssetName);


		}
		else
		{
			FY_CORE_WARN("Failed To Load File, File: {0}", TempFileName);
		}


		return returnValue;


	}


	std::string MotherLoader::GetFileName(const std::string& FilePath)
	{

		std::string temp_str = "";


		size_t count = (FilePath.size() - 1);
		while (FilePath[count] != '.')
		{
			count--;
		}

		count--;
		while (FilePath[count] != (char)'/')
		{
			temp_str.push_back(FilePath[count--]);
		}

		std::reverse(temp_str.begin(), temp_str.end());

		return temp_str;
	}






}


