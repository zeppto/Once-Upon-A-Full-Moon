#include <fypch.hpp>
#include "MotherLoader.hpp"
#include "Assetmanager.hpp"
#include "..\PrefabManager\PrefabManager.h"
#include"..\..\DEFINITIONS.hpp"


namespace Frosty
{

	MotherLoader* MotherLoader::s_Instance = nullptr;
	uint16_t MotherLoader::s_Failed_Loading_Attempts = 0;
	uint16_t MotherLoader::s_Total_Loading_Attempts = 0;
	uint16_t MotherLoader::s_Success_Loading_Attempts = 0;

	MotherLoader* MotherLoader::GetMotherLoader()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = FY_NEW MotherLoader;
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
	
	bool MotherLoader::Loadfile(const std::string& FilePath, const std::string& PrefabName,const bool& Reload)
	{
		bool returnValue = false;

		FileNameInfo TempFileInfo;
		TempFileInfo.m_FilePath = FilePath;
		TempFileInfo.m_PreFab_Name = PrefabName;

		if (GetFileInformation(TempFileInfo))
		{
			switch (TempFileInfo.m_type)
			{
			case JPG:
				returnValue = LoadGraphicFile(TempFileInfo,Reload);
				break;

			case PNG:
				returnValue = LoadGraphicFile(TempFileInfo,Reload);
				break;

			case LUNA:
				returnValue = LoadLunaFile(TempFileInfo,Reload);
				
				break;

			default:
				FY_CORE_WARN("Unknown fileformat, Filepath: {0}", TempFileInfo.m_FilePath);
				break;
			}
		}

		if (returnValue) 
		{
			s_Success_Loading_Attempts++;
		}
		else 
		{
			s_Failed_Loading_Attempts++;
		}
		s_Total_Loading_Attempts++;

		return returnValue;
	}

	void MotherLoader::LoadFiles()
	{
		using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

		for (const auto& dirEntry : recursive_directory_iterator(PROJECT_LUNAFILES_FOLDER_ROOT))
		{
			Loadfile(PROJECT_LUNAFILES_FOLDER_ROOT + dirEntry.path().filename().string());
		}
	}

	void MotherLoader::PrintLoadingAttemptInformation() const
	{
		FY_CORE_INFO("MotherLoader, Total Loading Attempts: {0}",s_Total_Loading_Attempts);
		FY_CORE_INFO("MotherLoader, Success Loading Attempts: {0}",s_Success_Loading_Attempts);
		FY_CORE_INFO("MotherLoader, Failed Loading Attempts: {0}",s_Failed_Loading_Attempts);
	}

	bool MotherLoader::GetFileInformation(FileNameInfo& FileNameInformation)
	{
		bool returnValue = false;


		std::string temp_Name = "";
		std::string temp_Type = "";


		size_t count = (FileNameInformation.m_FilePath.size() - 1);
		while (FileNameInformation.m_FilePath[count] != '.' && count > 0)
		{

			temp_Type.push_back(FileNameInformation.m_FilePath[count]);
			count--;
		}
		std::reverse(temp_Type.begin(), temp_Type.end());

		FileNameInformation.m_type = GetFileType(temp_Type);

		if (count > 0) {

			returnValue = true;

			count--;
			while (FileNameInformation.m_FilePath[count] != (char)'/')
			{
				temp_Name.push_back(FileNameInformation.m_FilePath[count--]);

				if (count < 0)
				{
					break;
				}

			}

			std::reverse(temp_Name.begin(), temp_Name.end());
			FileNameInformation.m_FileName = temp_Name;
		}
		else
		{
			FY_CORE_WARN("File Has No Name, Filepath {0}", FileNameInformation.m_FilePath);
		}
		return returnValue;
	}

	int8_t MotherLoader::GetFileType(const std::string& fileType) const
	{
		if (fileType == FILE_TYPE_JPG)
		{
			return JPG;
		}
		else if (fileType == FILE_TYPE_PNG)
		{
			return PNG;
		}
		else if (fileType == FILE_TYPE_LUNA)
		{
			return LUNA;
		}

		return -1;
	}

	bool MotherLoader::LoadLunaFile(const FileNameInfo& FileNameInformation, const bool& Reload)
	{

		bool returnValue = false;

		Luna::Reader tempFile;

		if (tempFile.readFile(FileNameInformation.m_FilePath.c_str()))
		{


			auto temp_AssetManager = Assetmanager::GetAssetmanager();

			ModelTemplate* mod_ptr = nullptr;


			//std::string Temp_MT_Asset_Name = TempFileName; //ModelName?

			//ModelTemplate
			if (temp_AssetManager->AddNewModelTemplate(mod_ptr, FileNameInformation.m_FileName, FileNameInformation.m_FilePath))
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

					FY_CORE_INFO("Trying To Reload a ModelTemplate: {0}", FileNameInformation.m_FileName);


				}
				else
				{
					FY_CORE_INFO("ModelTemplate Already Loaded, File: {0}", FileNameInformation.m_FileName);
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
				if (temp_AssetManager->AddNewMaterialTemplate(tempMatPtr, tempMatVector.at(i).diffuseTexPath, FileNameInformation.m_FileName))
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
						FY_CORE_INFO("Trying To Reload a Material: {0}", FileNameInformation.m_FileName);
					}
					else
					{
						FY_CORE_INFO("Material Already Loaded, File: {0}", FileNameInformation.m_FileName);
					}
				}
			}

			if (FileNameInformation.m_PreFab_Name != "")
			{
			PrefabManager::GetPrefabManager()->setPrefab(FileNameInformation.m_PreFab_Name, FileNameInformation.m_FileName, MaterialAssetName);
			}



		}
		else
		{
			FY_CORE_WARN("Luna Failed to load file, FilePath: {0}", FileNameInformation.m_FilePath);
		}
		return returnValue;
	}

	bool MotherLoader::LoadGraphicFile(const FileNameInfo& FileNameInformation, const bool& Reload)
	{
		bool returnValue = false;


		return returnValue;
	}






}


