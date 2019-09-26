#include <fypch.hpp>
#include "MotherLoader.hpp"
#include "Assetmanager.hpp"
#include "..\PrefabManager\PrefabManager.h"
#include"..\..\DEFINITIONS.hpp"
#include"stb_image.hpp"


bool GetImageSize(const char* fn, int* x, int* y)
{
	FILE* f = fopen(fn, "rb"); if (f == 0) return false;
	fseek(f, 0, SEEK_END); long len = ftell(f); fseek(f, 0, SEEK_SET);
	if (len < 24) { fclose(f); return false; }

	// Strategy:
	// reading GIF dimensions requires the first 10 bytes of the file
	// reading PNG dimensions requires the first 24 bytes of the file
	// reading JPEG dimensions requires scanning through jpeg chunks
	// In all formats, the file is at least 24 bytes big, so we'll read that always
	unsigned char buf[24]; fread(buf, 1, 24, f);

	// For JPEGs, we need to read the first 12 bytes of each chunk.
	// We'll read those 12 bytes at buf+2...buf+14, i.e. overwriting the existing buf.
	if (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF && buf[3] == 0xE0 && buf[6] == 'J' && buf[7] == 'F' && buf[8] == 'I' && buf[9] == 'F')
	{
		long pos = 2;
		while (buf[2] == 0xFF)
		{
			if (buf[3] == 0xC0 || buf[3] == 0xC1 || buf[3] == 0xC2 || buf[3] == 0xC3 || buf[3] == 0xC9 || buf[3] == 0xCA || buf[3] == 0xCB) break;
			pos += 2 + (buf[4] << 8) + buf[5];
			if (pos + 12 > len) break;
			fseek(f, pos, SEEK_SET); fread(buf + 2, 1, 12, f);
		}
	}

	fclose(f);

	// JPEG: (first two bytes of buf are first two bytes of the jpeg file; rest of buf is the DCT frame
	if (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF)
	{
		*y = (buf[7] << 8) + buf[8];
		*x = (buf[9] << 8) + buf[10];
		return true;
	}

	// GIF: first three bytes say "GIF", next three give version number. Then dimensions
	if (buf[0] == 'G' && buf[1] == 'I' && buf[2] == 'F')
	{
		*x = buf[6] + (buf[7] << 8);
		*y = buf[8] + (buf[9] << 8);
		return true;
	}

	// PNG: the first frame is by definition an IHDR frame, which gives dimensions
	if (buf[0] == 0x89 && buf[1] == 'P' && buf[2] == 'N' && buf[3] == 'G' && buf[4] == 0x0D && buf[5] == 0x0A && buf[6] == 0x1A && buf[7] == 0x0A
		&& buf[12] == 'I' && buf[13] == 'H' && buf[14] == 'D' && buf[15] == 'R')
	{
		*x = (buf[16] << 24) + (buf[17] << 16) + (buf[18] << 8) + (buf[19] << 0);
		*y = (buf[20] << 24) + (buf[21] << 16) + (buf[22] << 8) + (buf[23] << 0);
		return true;
	}

	return false;
}

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


		size_t count = (FileNameInformation.FilePath.size() - 1);
		while (FileNameInformation.FilePath[count] != '.' && count > 0)
		{

			temp_Type.push_back(FileNameInformation.FilePath[count]);
			count--;
		}
		std::reverse(temp_Type.begin(), temp_Type.end());

		FileNameInformation.Type = GetFileType(temp_Type);

		if (count > 0) {

			returnValue = true;

			count--;
			while (FileNameInformation.FilePath[count] != (char)'/')
			{
				temp_Name.push_back(FileNameInformation.FilePath[count--]);

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

		if (tempFile.readFile(FileNameInformation.FilePath.c_str()))
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

			std::shared_ptr<Luna::Material> tempMatPtr = nullptr;



			std::string MaterialAssetName = "HejHej Fel Fel";


			//Add Materials to holder
			for (int i = 0; i < tempMatVector.size(); i++)
			{

				//tempMatVector.at(i).diffuseTexPath Chop Name???

				//Load Textures to materials

				//Materials
				if (temp_AssetManager->AddNewMaterialTemplate(tempMatPtr, FileNameInformation))
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
	//	int comp;

	//	std::ifstream in(FileNameInformation.FilePath.c_str());

		 //int width, height;


		 //if (GetImageSize(FileNameInformation.FilePath.c_str(),&width,&height))
		 //{
			// FY_CORE_INFO("x: {0} y: {1}",width,height);
		 //}
		 //

		//in.seekg(16);
		//in.read((char*)& width, 4);
		//in.read((char*)& height, 4);
		//
		//width = ntohl(width);
		//height = ntohl(height);

	//	stbi_uc* tempTextuere = stbi_load(FileNameInformation.FilePath.c_str(), &width, &height, &comp, STBI_rgb);

	//	int width, height;



	//	int  channels;
	//	stbi_set_flip_vertically_on_load(true);


	//	std::string temp = FileNameInformation.FilePath;
		//temp.erase(FileNameInformation.FilePath.size() - 4, 4);

		//FY_CORE_INFO("Temp :{0}", temp);

		//unsigned char* image = stbi_load(temp.c_str(),
		//	&width,
		//	&height,
		//	&channels,
		//	STBI_rgb_alpha);




		int textureWidth, textureHeight, components;
		unsigned char* imageData = stbi_load((FileNameInformation.FilePath.c_str()), &textureWidth, &textureHeight, &components, STBI_rgb_alpha);
		
		if (imageData == nullptr)
		{
			//	std::cout << "Image failed to load in." << std::endl;
			//	FatalError();
			FY_CORE_WARN("Stbi Was unable to load Filepath: {0}", FileNameInformation.FilePath);
		}





		if (imageData != nullptr)
		{
			FY_CORE_INFO("Width: {0}, Height {1}", textureWidth,textureHeight);
		}
		else
		{
			FY_CORE_WARN("STBI Could not load image, FilePath: {0}", FileNameInformation.FilePath);
		}


		stbi_image_free(imageData);







		return returnValue;
	}

}


