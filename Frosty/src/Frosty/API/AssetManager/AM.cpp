#include"fypch.hpp"
#include "AM.hpp"
#include"Luna/include/Reader.h"
#include"..\..\DEFINITIONS.hpp"

namespace Frosty
{


	Assetmanager* Assetmanager::s_Instance = nullptr;
	uint16_t Assetmanager::s_Total_Nr_Assets = 0;


	std::unordered_map<std::string, Mesh> Assetmanager::s_Meshes;
	std::unordered_map<std::string, Animation> Assetmanager::s_Animaions;
	std::unordered_map<std::string, TextureFile> Assetmanager::s_Textures;
	std::unordered_map<std::string, LinkedMaterial> Assetmanager::s_LinkedMaterials;

	std::vector<std::string> Assetmanager::s_FilePath_Vector;
	//std::vector<std::string> Assetmanager::s_MeshNames;
	//std::vector<std::string> Assetmanager::s_AnimationsNames;
	//std::vector<std::string> Assetmanager::s_TexturesNames;
	//std::vector<std::string> Assetmanager::s_MaterialNames;

	uint16_t Assetmanager::s_Failed_Loading_Attempts = 0;
	uint16_t Assetmanager::s_Success_Loading_Attempts = 0;


	Assetmanager* Assetmanager::Get()
	{
		if (!s_Instance)
		{
			s_Instance = new Assetmanager;
		}
		return s_Instance;
	}

	Assetmanager::~Assetmanager()
	{

	}

	bool Assetmanager::LoadFile(const std::string& FullFilePath, const std::string& PreFab_Name)
	{
		bool returnValue = false;

		FileMetaData TempFileInfo;
		TempFileInfo.FullFilePath = FullFilePath;
		TempFileInfo.PreFab_Name = PreFab_Name;

		if (GetFileInformation(TempFileInfo))
		{
			switch (TempFileInfo.Type)
			{
			case JPG:
				returnValue = LoadGraphicFile(TempFileInfo);
				break;

			case PNG:
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
		using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

		for (const auto& dirEntry : recursive_directory_iterator(PROJECT_LUNAFILES_FOLDER_ROOT))
		{
			LoadFile(PROJECT_LUNAFILES_FOLDER_ROOT + dirEntry.path().filename().string());
		}
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
			return false;
		}
		else
		{
			s_Meshes[Mesh.GetLunaMesh().name] = Mesh;
		//	s_MeshNames.emplace_back(Mesh.GetLunaMesh().name);
		}
		return true;
	}

	bool Assetmanager::AddMesh(const FileMetaData& MetaData, const Luna::Mesh& LunMesh)
	{
		if (MeshLoaded(LunMesh.name))
		{
			return false;
		}
		else
		{
			s_Meshes[LunMesh.name] = Mesh(MetaData, LunMesh);
			//s_MeshNames.emplace_back(LunMesh.name);
		}
		return true;
	}

	bool Assetmanager::AddTexture(TextureFile& Tex)
	{
		if (TextureLoaded(Tex.GetfileMetaData().FileName))
		{
			return false;
		}
		else
		{
			s_Textures[Tex.GetfileMetaData().FileName] = Tex;
			//s_TexturesNames.emplace_back(Tex.GetfileMetaData().FileName);
		}
		return true;
	}

	bool Assetmanager::AddTexture(const FileMetaData& MetaData)
	{
		if (TextureLoaded(MetaData.FileName))
		{
			return false;
		}
		else
		{
			s_Textures[MetaData.FileName] = TextureFile(MetaData);
			//s_TexturesNames.emplace_back(MetaData.FileName);
		}
		return true;
	}

	bool Assetmanager::AddMaterial(LinkedMaterial& LnkMat)
	{
		if (MaterialLoaded(LnkMat.GetfileMetaData().FileName))
		{
			return false;
		}
		else
		{
			s_LinkedMaterials[LnkMat.GetfileMetaData().FileName] = LnkMat;
			//s_MaterialNames.emplace_back(LnkMat.GetfileMetaData().FileName);
		}
		return true;
	}

	bool Assetmanager::AddMaterial(const FileMetaData& MetaData, const Luna::Material& LunMat)
	{
		if (MaterialLoaded(MetaData.FileName))
		{
			return false;
		}
		else
		{
			s_LinkedMaterials[MetaData.FileName] = LinkedMaterial(MetaData, LunMat);
			//s_MaterialNames.emplace_back(MatName);
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
			//s_AnimationsNames.emplace_back(Animation.GetName());
		}
		return true;
	}

	bool Assetmanager::LoadMaterialTextures(LinkedMaterial*& Material)
	{

		LoadFile((PROJECT_LUNAFILES_FOLDER_ROOT + std::string(Material->GetMaterial().diffuseTexPath)));
		LoadFile((PROJECT_LUNAFILES_FOLDER_ROOT + std::string(Material->GetMaterial().normalTexPath)));
		LoadFile((PROJECT_LUNAFILES_FOLDER_ROOT + std::string(Material->GetMaterial().glowTexPath)));

		Material->GetGlow() = Assetmanager::GetTexture(CutFileExtentionFromString(Material->GetMaterial().diffuseTexPath));
		Material->GetNormal() = Assetmanager::GetTexture(CutFileExtentionFromString(Material->GetMaterial().diffuseTexPath));
		Material->GetDiffuse() = Assetmanager::GetTexture(CutFileExtentionFromString(Material->GetMaterial().diffuseTexPath));


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

		//std::unordered_map<std::string, LinkedMaterial>::iterator it0;
		//for (it0 = s_LinkedMaterials.begin(); it0 != s_LinkedMaterials.end() && returnValue == false; it0++)
		//for (std::pair<std::string, LinkedMaterial> element : s_LinkedMaterials)
		//{
		//	if (it0->first == FileName)
		//	{
		//		returnValue = true;
		//	}
		//}

		//std::unordered_map<std::string, AssetMetaData<ModelTemplate>>::iterator it1;
		//for (it1 = s_MT_MetaData_Map.begin(); it1 != s_MT_MetaData_Map.end() && returnValue == false; it1++)
		//{
		//	if (it1->first == FileName)
		//	{
		//		returnValue = true;
		//	}
		//}


		//std::unordered_map<std::string, AssetMetaData<TextureFile>>::iterator it2;
		//for (it2 = s_Texture_MetaData_Map.begin(); it2 != s_Texture_MetaData_Map.end() && returnValue == false; it2++)
		//{
		//	if (it2->first == FileName)
		//	{
		//		returnValue = true;
		//	}
		//}

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

		//for (int i = 0; i < s_MaterialNames.size() && returnValue == false; i++)
		//{
		//	if (s_MaterialNames.at(i) == FileName)
		//	{
		//		returnValue = true;
		//	}
		//}
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

		//for (int i = 0; i < s_TexturesNames.size() && returnValue == false; i++)
		//{
		//	if (s_TexturesNames.at(i) == FileName)
		//	{
		//		returnValue = true;
		//	}
		//}

		return returnValue;
	}

	bool Assetmanager::AnimationLoaded(const std::string& AssetName)
	{
		bool returnValue = false;



		//for (int i = 0; i < s_AnimationsNames.size() && returnValue == false; i++)
		//{
		//	if (s_AnimationsNames.at(i) == AssetName)
		//	{
		//		returnValue = true;
		//	}
		//}


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


		//for (int i = 0; i < s_MeshNames.size() && returnValue == false; i++)
		//{
		//	if (s_MeshNames.at(i) == AssetName)
		//	{
		//		returnValue = true;
		//	}
		//}

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

				if (Assetmanager::AddMesh(FileNameInformation, tempFile.getMesh(i)))
				{
					//Temp can be optimized
					Assetmanager::GetMesh(tempFile.getMesh(i).name)->LoadToMem();
					Assetmanager::GetMesh(tempFile.getMesh(i).name)->LoadToGPU();
				}

				if (tempFile.animationExist())
				{

					Assetmanager::AddAnimation(Animation(FileNameInformation, i, 1));
					//Temp can be optimized
					Assetmanager::GetAnimation(tempFile.getAnimation().animationName)->LoadToMem();
					Assetmanager::GetAnimation(tempFile.getAnimation().animationName)->LoadToGPU();

				}


				std::vector<Luna::Material> tempMatVector;
				tempFile.getMaterials(tempMatVector);

				for (int i = 0; i < tempMatVector.size(); i++)
				{
					FileMetaData TempMatMetaData = FileNameInformation;
					TempMatMetaData.FileName = "Mat_" + std::to_string(i) + ":" + FileNameInformation.FileName;

					AddMaterial(TempMatMetaData, tempFile.getMaterial(i));
					//Temp can be optimized
					LinkedMaterial* kk = GetMaterial(TempMatMetaData.FileName);
					LoadMaterialTextures(kk);
					
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

		if (Assetmanager::AddTexture(FileNameInformation))
		{
			//Temp
			Assetmanager::GetTexture(FileNameInformation.FileName)->LoadToMem();
			Assetmanager::GetTexture(FileNameInformation.FileName)->LoadToGPU();
		}
		else
		{
			FY_CORE_WARN("(MotherLoader) Could not load Image, Name: {0}", FileNameInformation.FileName);
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
			FY_CORE_WARN("(MotherLoader) File Has No Name, Filepath {0}", FileNameInformation.FileName);
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

		return -1;
	}

	const std::string Assetmanager::CutFileExtentionFromString(const char* in_char_ptr)
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