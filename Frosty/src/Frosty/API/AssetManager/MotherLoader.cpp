#include <fypch.hpp>
#include "MotherLoader.hpp"
#include "Assetmanager.hpp"
#include "..\PrefabManager\PrefabManager.h"
#include"..\..\DEFINITIONS.hpp"
#include "Frosty/Core/Application.hpp"

namespace Frosty
{

	MotherLoader* MotherLoader::s_Instance = nullptr;
	uint16_t MotherLoader::s_Failed_Loading_Attempts = 0;
	uint16_t MotherLoader::s_Success_Loading_Attempts = 0;

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
	
	bool MotherLoader::Loadfile(const std::string& FilePath, const std::string& PrefabName,const bool& Reload)
	{
		bool returnValue = false;

		FileMetaData TempFileInfo;
		TempFileInfo.FullFilePath = FilePath;
		TempFileInfo.PreFab_Name = PrefabName;

		if (GetFileInformation(TempFileInfo))
		{
			switch (TempFileInfo.Type)
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
				FY_CORE_WARN("Unknown fileformat, Filepath: {0}", TempFileInfo.FilePath);
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
		FY_CORE_INFO("________________________________________________________");
		FY_CORE_INFO("MotherLoader, Success Loading Attempts: {0}",s_Success_Loading_Attempts);
		FY_CORE_INFO("MotherLoader, Failed Loading Attempts : {0}",s_Failed_Loading_Attempts);
		FY_CORE_INFO("-----------------------------------------=--------------");
		FY_CORE_INFO("MotherLoader, Total Loading Attempts  : {0}", (s_Success_Loading_Attempts + s_Failed_Loading_Attempts));
		FY_CORE_INFO("¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯");
	}

	bool MotherLoader::GetFileInformation(FileMetaData& FileNameInformation)
	{
		bool returnValue = false;


		std::string temp_Name = "";
		std::string temp_Type = "";

		size_t count = (FileNameInformation.FullFilePath.size()-1);
		while (FileNameInformation.FullFilePath[count] != '.' && count > 0)
		{

			temp_Type.push_back(FileNameInformation.FullFilePath[count]);
			count--;
		}
		std::reverse(temp_Type.begin(), temp_Type.end());

		FileNameInformation.Type = GetFileType(temp_Type);

		if (count > 0) {

			returnValue = true;

			count--;
			while (FileNameInformation.FullFilePath[count] != (char)'/')
			{
				temp_Name.push_back(FileNameInformation.FullFilePath[count--]);

				if (count < 0)
				{
					break;
				}

			}

			std::reverse(temp_Name.begin(), temp_Name.end());
			FileNameInformation.FileName = temp_Name;
			FileNameInformation.FilePath = temp_Name + "." + temp_Type;
		}
		else
		{
			FY_CORE_WARN("File Has No Name, Filepath {0}", FileNameInformation.FilePath);
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

	bool MotherLoader::LoadLunaFile(const FileMetaData& FileNameInformation, const bool& Reload)
	{

		bool returnValue = false;

		Luna::Reader tempFile;

		if (tempFile.readFile(FileNameInformation.FullFilePath.c_str()))
		{


			auto temp_AssetManager = Assetmanager::GetAssetmanager();

			std::shared_ptr<ModelTemplate> mod_ptr = nullptr;


			//std::string Temp_MT_Asset_Name = TempFileName; //ModelName?

			//ModelTemplate
			if (temp_AssetManager->AddNewModelTemplate(mod_ptr, FileNameInformation))
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
						tempFile.getWeights(tempMeshId, tempMeshInfo_Ptr->Weights);
						modelHasSkeleton = true;
					}

					if (mod_ptr->GetMeshVector()->back().hasBoundingBox)
					{
						tempMeshInfo_Ptr->BoundingBox = tempFile.getBoundingBox(tempMeshId);
					}

					tempFile.getIndices(tempMeshId, tempMeshInfo_Ptr->MeshIndices);
					tempFile.getVertices(tempMeshId, tempMeshInfo_Ptr->MeshVertices);

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




				mod_ptr->LoadModelToGpu();

			}
			else
			{
				if (Reload)
				{

					FY_CORE_INFO("Trying To Reload a ModelTemplate: {0}", FileNameInformation.FileName);


				}
				else
				{
					FY_CORE_INFO("ModelTemplate Already Loaded, File: {0}", FileNameInformation.FileName);
				}


			}



			//Get Material Names
			std::vector<Luna::Material> tempMatVector;
			tempFile.getMaterials(tempMatVector);

			std::shared_ptr<LinkedMaterial> tempMatPtr = nullptr;



			std::string MaterialAssetName = "Could not Load Material";


			//Add Materials to holder
			for (int i = 0; i < tempMatVector.size(); i++)
			{

				//tempMatVector.at(i).diffuseTexPath Chop Name???

				//Load Textures to materials

				//Materials

				FileMetaData TempMatMetaData = FileNameInformation;
				TempMatMetaData.FileName = "Mat_" + std::to_string(i) + ":" + FileNameInformation.FileName;

				if (temp_AssetManager->AddNewMaterialTemplate(tempMatVector.at(i), TempMatMetaData))
				{
					//Fill Material

					//saving latest material name for prefab(If Needed, build so the prefab can support more materials)
					MaterialAssetName = TempMatMetaData.FileName;

				}
				else
				{
					if (Reload)
					{
						FY_CORE_INFO("Trying To Reload a Material: {0}", FileNameInformation.FileName);
					}
					else
					{
						FY_CORE_INFO("Material Already Loaded, File: {0}", FileNameInformation.FileName);
					}
				}
			}

			if (FileNameInformation.PreFab_Name != "")
			{
			PrefabManager::GetPrefabManager()->setPrefab(FileNameInformation.PreFab_Name, FileNameInformation.FileName, MaterialAssetName);
			}



		}
		else
		{
			FY_CORE_WARN("Luna Failed to load file, FilePath: {0}", FileNameInformation.FilePath);
		}
		return returnValue;
	}

	bool MotherLoader::LoadGraphicFile(const FileMetaData& FileNameInformation, const bool& Reload)
	{
		bool returnValue = false;



			std::shared_ptr<TextureFile> tempTexturePtr = Assetmanager::GetAssetmanager()->AddNewTextureTemplate(FileNameInformation);

			if (tempTexturePtr != nullptr)
			{
				if (tempTexturePtr->LoadToGpu())
				{
					returnValue = true;
				}
				else
				{
					FY_CORE_WARN("Could not load Image to GPU, Name: {0}", FileNameInformation.FileName);
				}
			}
			else
			{
				FY_CORE_WARN("Could not load Image, Name: {0}" , FileNameInformation.FileName);
			}
		


		return returnValue;
	}

}


