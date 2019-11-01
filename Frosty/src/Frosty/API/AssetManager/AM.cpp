#include"fypch.hpp"
#include "AM.hpp"
#include"Luna/include/Reader.h"
#include"..\..\DEFINITIONS.hpp"

#include <cstring>
#include <io.h>
#include <stdio.h>
#include<direct.h>

namespace Frosty
{

	bool Assetmanager::s_AutoLoad = true;
	Assetmanager* Assetmanager::s_Instance = nullptr;
	uint16_t Assetmanager::s_Total_Nr_Assets = 0;

	std::unordered_map<std::string, Mesh> Assetmanager::s_Meshes;
	std::unordered_map<std::string, Animation> Assetmanager::s_Animaions;
	std::unordered_map<std::string, TextureFile> Assetmanager::s_Textures;
	std::unordered_map<std::string, LinkedMaterial> Assetmanager::s_LinkedMaterials;
	std::unordered_map <std::string, std::list<TextureFile**>> Assetmanager::s_TextureWatchList;

	std::vector<std::string> Assetmanager::s_FilePath_Vector;

	uint16_t Assetmanager::s_Failed_Loading_Attempts = 0;
	uint16_t Assetmanager::s_Success_Loading_Attempts = 0;


	Assetmanager* Assetmanager::Get()
	{
		if (!s_Instance)
		{
			s_Instance = FY_NEW Assetmanager;
		}
		return s_Instance;
	}

	Assetmanager::~Assetmanager()
	{

	}

	bool Assetmanager::LoadFile(const std::string& FullFilePath, const std::string& TagName)
	{
		bool returnValue = false;

		FileMetaData TempFileInfo;
		TempFileInfo.FullFilePath = FullFilePath;
		TempFileInfo.TagName = TagName;

		if (GetFileInformation(TempFileInfo))
		{
			switch (TempFileInfo.Type)
			{
			case JPG :

				returnValue = LoadGraphicFile(TempFileInfo);
				break;
			case PNG:
				returnValue = LoadGraphicFile(TempFileInfo);
				break;

			case TGA:
				returnValue = LoadGraphicFile(TempFileInfo);
				break;

			case LUNA:
				returnValue = LoadLunaFile(TempFileInfo);

				break;



			default:
				FY_CORE_WARN("Unknown fileformat, Filepath: {0}", TempFileInfo.FileName);
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

	void Assetmanager::LoadFiles()
	{
		LoadDir("assets/");
		ConnectWatchList();
	}

	void Assetmanager::PrintLoadInfo() const
	{
		FY_CORE_INFO("________________________________________________________");
		FY_CORE_INFO("MotherLoader, Success Loading Attempts: {0}", s_Success_Loading_Attempts);
		FY_CORE_INFO("MotherLoader, Failed Loading Attempts : {0}", s_Failed_Loading_Attempts);
		FY_CORE_INFO("-----------------------------------------=--------------");
		FY_CORE_INFO("MotherLoader, Total Loading Attempts  : {0}", (s_Success_Loading_Attempts + s_Failed_Loading_Attempts));
		FY_CORE_INFO("¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯");
	}

	bool Assetmanager::LinkKey(const std::string& AssetName, BaseKey* In_Key)
	{
		bool returnValue = false;

		if (MaterialLoaded(AssetName))
		{

			KeyLabel<LinkedMaterial>* temp_mt_ptr = static_cast<KeyLabel<LinkedMaterial>*>(In_Key);
			if (temp_mt_ptr != nullptr)
			{
				temp_mt_ptr->SetKeyData(s_LinkedMaterials[AssetName]);
				returnValue = true;
			}
			else
			{
				FY_CORE_WARN("Could not link key, Key was Not Correct type,Asset Name: {0}", AssetName);
			}
		}
		else if (TextureLoaded(AssetName))
		{

			KeyLabel<TextureFile>* temp_mt_ptr = static_cast<KeyLabel<TextureFile>*>(In_Key);
			if (temp_mt_ptr != nullptr)
			{
				temp_mt_ptr->SetKeyData(s_Textures[AssetName]);
				returnValue = true;
			}
			else
			{
				FY_CORE_WARN("Could not link key, Key was Not Correct type,Asset Name: {0}", AssetName);
			}
		}
		else if (AnimationLoaded(AssetName))
		{

			KeyLabel<Animation>* temp_mt_ptr = static_cast<KeyLabel<Animation>*>(In_Key);
			if (temp_mt_ptr != nullptr)
			{
				temp_mt_ptr->SetKeyData(s_Animaions[AssetName]);
				returnValue = true;
			}
			else
			{
				FY_CORE_WARN("Could not link key, Key was Not Correct type,Asset Name: {0}", AssetName);
			}
		}
		else if (MeshLoaded(AssetName))
		{
			KeyLabel<Mesh>* temp_mt_ptr = static_cast<KeyLabel<Mesh>*>(In_Key);
			if (temp_mt_ptr != nullptr)
			{
				temp_mt_ptr->SetKeyData(s_Meshes[AssetName]);
				returnValue = true;
			}
			else
			{
				FY_CORE_WARN("Could not link key, Key was Not Correct type,Asset Name: {0}", AssetName);
			}

		}
		else
		{
			FY_CORE_WARN("Could not link key, The Asset is not loaded, Name: {0}", AssetName);
		}
		return returnValue;
	}

	bool Assetmanager::AddMesh(Mesh& Mesh)
	{
		if (MeshLoaded(Mesh.GetLunaMesh().name))
		{
			FY_CORE_INFO("Mesh name: {0}, Is already loaded", Mesh.GetLunaMesh().name);
			return false;
		}
		else
		{
			s_Meshes[Mesh.GetLunaMesh().name] = Mesh;
		}
		return true;
	}

	bool Assetmanager::AddMesh(const FileMetaData& MetaData, const Luna::Mesh& LunMesh)
	{
		if (MeshLoaded(LunMesh.name))
		{
			FY_CORE_INFO("Mesh name: {0}, Is already loaded", LunMesh.name);
			return false;
		}
		else
		{
			s_Meshes[LunMesh.name] = Mesh(MetaData, LunMesh);
		}
		return true;
	}

	bool Assetmanager::AddTexture(TextureFile& Tex)
	{
		if (TextureLoaded(Tex.GetfileMetaData().FileName))
		{
			FY_CORE_INFO("Texture: {0}, Is already loaded", Tex.GetfileMetaData().FileName);
			return false;
		}
		else
		{
			s_Textures[Tex.GetfileMetaData().FileName] = Tex;
		}
		return true;
	}

	bool Assetmanager::AddTexture(const FileMetaData& MetaData)
	{
		if (TextureLoaded(MetaData.FileName))
		{
			FY_CORE_INFO("Texture: {0}, Is already loaded", MetaData.FileName);
			return false;
		}
		else
		{
			s_Textures[MetaData.FileName] = TextureFile(MetaData);
		}
		return true;
	}

	bool Assetmanager::AddMaterial(LinkedMaterial& LnkMat)
	{
		if (MaterialLoaded(LnkMat.GetfileMetaData().FileName))
		{
			FY_CORE_INFO("Material: {0}, Is already loaded", LnkMat.GetfileMetaData().FileName);
			return false;
		}
		else
		{
			s_LinkedMaterials[LnkMat.GetfileMetaData().FileName] = LnkMat;
		}
		return true;
	}

	bool Assetmanager::AddMaterial(const FileMetaData& MetaData, const Luna::Material& LunMat)
	{
		if (MaterialLoaded(MetaData.FileName))
		{
			FY_CORE_INFO("Material: {0}, Is already loaded", MetaData.FileName);
			return false;
		}
		else
		{
			s_LinkedMaterials[MetaData.FileName] = LinkedMaterial(MetaData, LunMat);
		}
		return true;
	}

	bool Assetmanager::AddAnimation(Animation& Animation)
	{
		if (AnimationLoaded(Animation.GetName()))
		{
			return false;
		}
		else
		{
			s_Animaions[Animation.GetName()] = Animation;
		}
		return true;
	}

	bool Assetmanager::FileLoaded(const std::string& FilePath)
	{
		bool returnValue = false;

		for (int i = 0; i < s_FilePath_Vector.size() && returnValue == false; i++)
		{
			if (s_FilePath_Vector.at(i) == FilePath)
			{
				returnValue = true;
			}
		}
		return returnValue;
	}

	bool Assetmanager::AssetLoaded(const std::string& AssetName)
	{
		bool returnValue = false;

		if (!returnValue)
		{
			returnValue = MaterialLoaded(AssetName);
		}
		if (!returnValue)
		{
			returnValue = TextureLoaded(AssetName);
		}
		if (!returnValue)
		{
			returnValue = AnimationLoaded(AssetName);
		}
		if (!returnValue)
		{
			returnValue = MeshLoaded(AssetName);
		}

		return returnValue;
	}

	bool Assetmanager::MaterialLoaded(const std::string& FileName)
	{
		bool returnValue = false;


		std::unordered_map<std::string, LinkedMaterial>::iterator it;
		for (it = s_LinkedMaterials.begin(); it != s_LinkedMaterials.end() && returnValue == false; it++)
		{
			if (it->first == FileName)
			{
				returnValue = true;
			}
		}

		return returnValue;
	}

	bool Assetmanager::TextureLoaded(const std::string& FileName)
	{
		bool returnValue = false;

		std::unordered_map<std::string, TextureFile>::iterator it;
		for (it = s_Textures.begin(); it != s_Textures.end() && returnValue == false; it++)
		{
			if (it->first == FileName)
			{
				returnValue = true;
			}
		}

		return returnValue;
	}

	bool Assetmanager::AnimationLoaded(const std::string& AssetName)
	{
		bool returnValue = false;

		std::unordered_map<std::string, Animation>::iterator it;
		for (it = s_Animaions.begin(); it != s_Animaions.end() && returnValue == false; it++)
		{
			if (it->first == AssetName)
			{
				returnValue = true;
			}
		}

		return returnValue;
	}

	bool Assetmanager::MeshLoaded(const std::string& AssetName)
	{
		bool returnValue = false;


		std::unordered_map<std::string, Mesh>::iterator it;
		for (it = s_Meshes.begin(); it != s_Meshes.end() && returnValue == false; it++)
		{
			if (it->first == AssetName)
			{
				returnValue = true;
			}
		}

		return returnValue;
	}

	bool Assetmanager::LoadLunaFile(const FileMetaData& FileNameInformation, const bool& Reload)
	{

		bool returnValue = false;

		Luna::Reader tempFile;

		if (tempFile.readFile(FileNameInformation.FullFilePath.c_str()))
		{

			//the get functions can be skipped for optimization

			returnValue = true;
			bool modelHasSkeleton = false;

			// for nr of meshes
			for (uint16_t i = 0; i < tempFile.getMeshCount(); i++)
			{

				if (AddMesh(FileNameInformation, tempFile.getMesh(i)))
				{
					if (s_AutoLoad)
					{
						//Temp can be optimized
						GetMesh(tempFile.getMesh(i).name)->LoadToMem();
						GetMesh(tempFile.getMesh(i).name)->LoadToGPU();
					}
				}

				if (tempFile.animationExist())
				{

					AddAnimation(Animation(FileNameInformation, i, 1));
					if (s_AutoLoad)
					{
						//Temp can be optimized
						GetAnimation(tempFile.getAnimation().animationName)->LoadToMem();
						GetAnimation(tempFile.getAnimation().animationName)->LoadToGPU();
					}
				}




				//Material
				std::vector<Luna::Material> tempMatVector;
				tempFile.getMaterials(tempMatVector);

				for (int i = 0; i < tempMatVector.size(); i++)
				{
					FileMetaData TempMatMetaData = FileNameInformation;
					TempMatMetaData.FileName = MAT_NAME + std::to_string(i) + MAT_NAME_FOLLOW + FileNameInformation.FileName;

					if (AddMaterial(TempMatMetaData, tempFile.getMaterial(i)))
					{
						LinkedMaterial* tempLnk = GetMaterial(TempMatMetaData.FileName);
						TextureFile* tempTexfile = nullptr;
						int8_t tempType = -1;

						//Diffuse
						std::string tempFileName = CutFileExtention(tempLnk->GetMaterial().diffuseTexPath);
						if (tempFileName != "")
						{
							tempTexfile = GetTexture(tempFileName);
							if (tempTexfile == nullptr)
							{
								AddTexPtrToWatchList(tempFileName,tempLnk->GetDiffuse());
							}
							else
							{
								//Temp can be optimized
								GetMaterial(TempMatMetaData.FileName)->SetDiffuse(*tempTexfile);
							}

						}
						else
						{
							FY_CORE_WARN("Luna File: {0}, Does not have a diffuse texture", TempMatMetaData.FileName);
						}


						//Normal
						if (tempLnk->GetMaterial().hasNormalMap)
						{
							tempFileName = CutFileExtention(tempLnk->GetMaterial().normalTexPath);
							if (tempFileName != "")
							{
								tempTexfile = GetTexture(tempFileName);
								if (tempTexfile == nullptr)
								{
									AddTexPtrToWatchList(tempFileName, tempLnk->GetNormal());
								}
								else
								{
									//Temp can be optimized
									GetMaterial(TempMatMetaData.FileName)->SetNormal(*tempTexfile);
								}

							}
							else
							{
								FY_CORE_WARN("Luna File: {0}, Does not have a normal texture (Fault when exported file from Luna)", TempMatMetaData.FileName);
							}
						}


						//Glow
						if (tempLnk->GetMaterial().hasGlowMap)
						{
							tempFileName = CutFileExtention(tempLnk->GetMaterial().glowTexPath);
							if (tempFileName != "")
							{
								tempTexfile = GetTexture(tempFileName);
								if (tempTexfile == nullptr)
								{
									AddTexPtrToWatchList(tempFileName, tempLnk->GetGlow());
								}
								else
								{
									//Temp can be optimized
									GetMaterial(TempMatMetaData.FileName)->SetGlow(*tempTexfile);
								}
							}
							else
							{
								FY_CORE_WARN("Luna File: {0}, Does not have a Glow texture (Fault when exported file from Luna)", TempMatMetaData.FileName);
							}
						}
					}
				}
			}
		}
		else
		{
			FY_CORE_WARN("Luna Failed to load file, FilePath: {0}", FileNameInformation.FileName);
		}
		return returnValue;
	}

	bool Assetmanager::LoadGraphicFile(const FileMetaData& FileNameInformation, const bool& Reload)
	{
		bool returnValue = false;

		if (AddTexture(FileNameInformation))
		{
			if (s_AutoLoad)
			{
				//Temp
				GetTexture(FileNameInformation.FileName)->LoadToMem();
				GetTexture(FileNameInformation.FileName)->LoadToGPU();
				GetTexture(FileNameInformation.FileName)->DeleteFromMem();
			}
			returnValue = true;
		}

		return returnValue;
	}

	bool Assetmanager::GetFileInformation(FileMetaData& FileNameInformation)
	{
		bool returnValue = false;

		std::string temp_Name = "";
		std::string temp_Type = "";

		size_t count = (FileNameInformation.FullFilePath.size() - 1);
		while (FileNameInformation.FullFilePath[count] != '.' && count > 0)
		{

			temp_Type.push_back(FileNameInformation.FullFilePath[count]);
			count--;
		}
		std::reverse(temp_Type.begin(), temp_Type.end());

		//can use CutFileName
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
			FileNameInformation.FileExtentionName = "." + temp_Type;
		}
		else
		{
			FY_CORE_WARN("File Has No Name, Filepath {0}", FileNameInformation.FullFilePath);
		}
		return returnValue;
	}

	int8_t Assetmanager::GetFileType(const std::string& fileType) const
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
		else if (fileType == FILE_TYPE_TGA)
		{
			return TGA;
		}
		else if (fileType == FILE_TYPE_GLSL)
		{
			return GLSL;
		}


		return -1;
	}

	void Assetmanager::LoadDir(const std::string& dir)
	{
		for (const auto& dirEntry : directory_iterator(dir))
		{
			if (dirEntry.is_directory())
			{
				LoadDir((dir + dirEntry.path().filename().string() + "/"));
			}
			else
			{
				LoadFile(dirEntry.path().string());
			}
		}
	}

	void Assetmanager::ConnectWatchList()
	{
		std::unordered_map<std::string, TextureFile>::iterator it = s_Textures.begin();

		while (it != s_Textures.end())
		{
			std::list<TextureFile**>::iterator L_it = s_TextureWatchList[it->first].begin();

			while (L_it != s_TextureWatchList[it->first].end())
			{
				**L_it = &it->second;
				L_it++;
			}
			it++;
		}
		s_TextureWatchList.erase(s_TextureWatchList.begin(), s_TextureWatchList.end());
	}

	const std::string Assetmanager::CutFileName(const char* in_char_ptr)
	{
		std::string returnString = "";

		if (*in_char_ptr != '\0')
		{

			uint8_t count = 0;
			bool startPush = false;
			while (in_char_ptr[count] != '\0')
			{
				if (startPush)
				{
					returnString.push_back(in_char_ptr[count]);
				}

				if (in_char_ptr[count] == '.')
				{
					returnString = "";
					startPush = true;
				}
				count++;
			}
		}
		else
		{
			FY_CORE_WARN("Trying to cut a file extention from a emtpy char ptr");
		}
		return returnString;
	}


	const std::string Assetmanager::CutFileExtention(const char* in_char_ptr)
	{
		std::string returnString = "";


		if (*in_char_ptr != '\0')
		{

			uint8_t count = 0;
			while (in_char_ptr[count] != '\0')
			{
				if (in_char_ptr[count] != '.')
				{
					returnString.push_back(in_char_ptr[count]);
				}
				else
				{
					break;
				}
				count++;
			}
		}
		else
		{
			FY_CORE_WARN("Trying to cut a file extention from a emtpy char ptr");
		}
		return returnString;
	}

}