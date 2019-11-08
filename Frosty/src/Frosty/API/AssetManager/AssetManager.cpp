#include"fypch.hpp"
#include "AssetManager.hpp"
#include"Luna/include/Reader.h"
#include"..\..\DEFINITIONS.hpp"

#include <cstring>
#include <io.h>
#include <stdio.h>
#include<direct.h>

namespace Frosty
{

	bool AssetManager::s_AutoLoad = true;
	AssetManager* AssetManager::s_Instance = nullptr;
	uint16_t AssetManager::s_Total_Nr_Assets = 0;

	std::vector<std::string> AssetManager::testStr;

	std::unordered_map<std::string, Mesh> AssetManager::s_Meshes;
	std::unordered_map<std::string, Animation> AssetManager::s_Animaions;
	std::unordered_map<std::string, TextureFile> AssetManager::s_Textures;
	std::unordered_map<std::string, LinkedMaterial> AssetManager::s_LinkedMaterials;
	std::unordered_map <std::string, std::list<TextureFile**>> AssetManager::s_TextureWatchList;

	//From Temp AssetManager
	std::map<std::string, std::shared_ptr<VertexArray>> AssetManager::s_VertexArrays;
	std::map<std::string, std::shared_ptr<Shader>> AssetManager::s_Shaders;
	std::map<std::string, std::shared_ptr<Texture2D>> AssetManager::s_Textures2D;
	std::map<std::string, std::shared_ptr<Luna::BoundingBox>> AssetManager::s_BoundingBoxes;

	std::vector<std::string> AssetManager::s_FilePath_Vector;

	uint16_t AssetManager::s_Failed_Loading_Attempts = 0;
	uint16_t AssetManager::s_Success_Loading_Attempts = 0;


	AssetManager* AssetManager::Get()
	{
		if (!s_Instance)
		{
			s_Instance = FY_NEW AssetManager;
		}
		return s_Instance;
	}

	AssetManager::~AssetManager()
	{

	}

	bool AssetManager::LoadFile(const std::string& FullFilePath, const std::string& TagName)
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
				if (TempFileInfo.FileName == "Tree1")
				{
					int f = 0;
				}

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

	void AssetManager::LoadFiles()
	{
		s_Shaders.emplace("FlatColor", FY_NEW Shader("assets/shaders/FlatColorVertex.glsl", "assets/shaders/FlatColorFragment.glsl", "FlatColor"));
		s_Shaders.emplace("Texture2D", FY_NEW Shader("assets/shaders/Texture2DVertex.glsl", "assets/shaders/Texture2DFragment.glsl", "Texture2D"));
		s_Shaders.emplace("BlendShader", FY_NEW Shader("assets/shaders/BlendShaderVertex.glsl", "assets/shaders/BlendShaderFragment.glsl", "BlendShader"));
		s_Shaders.emplace("UI", FY_NEW Shader("assets/shaders/UIVertex.glsl", "assets/shaders/UIFragment.glsl", "UI"));
		s_Shaders.emplace("Particles", FY_NEW Shader("assets/shaders/ParticleVertex.glsl", "assets/shaders/ParticleGeometry.glsl", "assets/shaders/ParticleFragment.glsl", "Particles"));

		//Don't try to apply a compute shader as a material! This might have to be separate from normal shaders just to avoid confusion.
		s_Shaders.emplace("ParticleCompute", FY_NEW Shader("assets/shaders/ParticleCompute.glsl", "ParticleCompute"));

		s_Shaders["Texture2D"]->Bind();

		// Clock
		s_Shaders["Texture2D"]->UploadUniformInt("u_DiffuseTexture", 0);
		s_Shaders["Texture2D"]->UploadUniformInt("u_NormalTexture", 1);
		s_Shaders["Texture2D"]->UploadUniformInt("u_SpecularTexture", 2);

		s_Shaders["BlendShader"]->Bind();
		s_Shaders["BlendShader"]->UploadUniformInt("u_DiffuseTexture", 0);
		s_Shaders["BlendShader"]->UploadUniformInt("u_NormalTexture", 1);
		s_Shaders["BlendShader"]->UploadUniformInt("u_SpecularTexture", 2);
		s_Shaders["BlendShader"]->UploadUniformInt("u_BlendMapTexture", 3);
		s_Shaders["BlendShader"]->UploadUniformInt("u_BlendTexture1", 4);
		s_Shaders["BlendShader"]->UploadUniformInt("u_BlendTexture2", 5);


		s_Shaders["UI"]->Bind();

		s_Shaders["UI"]->UploadUniformInt("u_DiffuseTexture", 0);


		LoadDir("assets/");
		ConnectWatchList();


		for (int i = 0; i < testStr.size(); i++) {

			FY_CORE_INFO("Name:{0}", testStr.at(i));

		}
			

	}

	bool AssetManager::AddMesh(const FileMetaData& MetaData, const std::vector<Luna::Vertex>& vertices, const std::vector<Luna::Index>& indices)
	{
		bool returnValue = false;
		if (!MeshLoaded(MetaData.TagName))
		{
			// Vertex Array
			s_VertexArrays.emplace(MetaData.TagName, VertexArray::Create());

			// Vertex Buffer
			std::shared_ptr<VertexBuffer> vertexBuffer;
			vertexBuffer.reset(VertexBuffer::Create(&vertices.front(), sizeof(Luna::Vertex) * (uint32_t)vertices.size(), BufferType::STATIC));

			BufferLayout layout = {
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TextureCoords" },
				{ ShaderDataType::Float3, "a_Normal" },
				{ ShaderDataType::Float3, "a_Tangent" },
				{ ShaderDataType::Float3, "a_Bitangent" }
			};
			vertexBuffer->SetLayout(layout);

			s_VertexArrays[MetaData.TagName]->AddVertexBuffer(vertexBuffer);

			// Index Buffer
			std::shared_ptr<IndexBuffer> indexBuffer;
			indexBuffer.reset(IndexBuffer::Create(&indices.front(), (uint32_t)indices.size()));
			s_VertexArrays[MetaData.TagName]->SetIndexBuffer(indexBuffer);
			returnValue = true;
		}
		else
		{
			FY_CORE_WARN("Mesh: {0}, Is already loaded", MetaData.TagName);
		}
		return returnValue;
	}

	bool AssetManager::AddTexture(const FileMetaData& MetaData)
	{
		if (TextureLoaded(MetaData.FileName))
		{
			FY_CORE_INFO("Texture: {0}, Is already loaded", MetaData.FileName);
			return false;
		}
		else
		{
			s_Textures2D.emplace(MetaData.FileName, FY_NEW Texture2D(MetaData.FileName, MetaData.FullFilePath));
		}
		return true;
	}

	bool AssetManager::AddMaterial(LinkedMaterial& LnkMat)
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

	bool AssetManager::AddMaterial(const FileMetaData& MetaData, const Luna::Material& LunMat)
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

	bool AssetManager::AddAnimation(Animation& Animation)
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

	bool AssetManager::AddBoundingbox(const FileMetaData& MetaData, const Luna::BoundingBox& Boundinbox)
	{
		bool returnValue = false;
		if (!BoundingboxLoaded(MetaData.TagName))
		{
			s_BoundingBoxes[MetaData.TagName] = std::make_shared<Luna::BoundingBox>(Boundinbox);
		}
		else
		{
			FY_CORE_WARN("BoundingBox: {0}, Is already loaded", MetaData.TagName);
		}
		return returnValue;
	}

	bool AssetManager::FileLoaded(const std::string& FilePath)
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

	bool AssetManager::AssetLoaded(const std::string& AssetName)
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

	bool AssetManager::MaterialLoaded(const std::string& FileName)
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

	bool AssetManager::TextureLoaded(const std::string& FileName)
	{
		bool returnValue = false;

		std::map<std::string, std::shared_ptr<Texture2D>>::iterator it;
		for (it = s_Textures2D.begin(); it != s_Textures2D.end() && returnValue == false; it++)
		{
			if (it->first == FileName)
			{
				returnValue = true;
			}
		}

		return returnValue;
	}

	bool AssetManager::AnimationLoaded(const std::string& AssetName)
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

	bool AssetManager::MeshLoaded(const std::string& AssetName)
	{
		bool returnValue = false;


		std::map<std::string, std::shared_ptr<VertexArray>>::iterator it;
		for (it = s_VertexArrays.begin(); it != s_VertexArrays.end() && returnValue == false; it++)
		{
			if (it->first == AssetName)
			{
				returnValue = true;
			}
		}

		return returnValue;
	}

	bool AssetManager::BoundingboxLoaded(const std::string& MeshName)
	{
		bool returnValue = false;

		std::map<std::string, std::shared_ptr<Luna::BoundingBox>>::iterator it;
		for (it = s_BoundingBoxes.begin(); it != s_BoundingBoxes.end() && returnValue == false; it++)
		{
			if (it->first == MeshName)
			{
				returnValue = true;
			}
		}

		return returnValue;
	}


	bool AssetManager::LoadLunaFile(const FileMetaData& FileNameInformation, const bool& Reload)
	{

		bool returnValue = false;

		Luna::Reader tempFile;

		if (tempFile.readFile(FileNameInformation.FullFilePath.c_str()))
		{

			returnValue = true;
			bool modelHasSkeleton = false;

			// for nr of meshes
			for (uint16_t i = 0; i < tempFile.getMeshCount(); i++)
			{


				//Mesh
				std::vector<Luna::Vertex> vertices;
				std::vector<Luna::Index> indices;

				FileMetaData tempMetaData = FileNameInformation;
				tempMetaData.TagName = CharToStr(tempFile.getMesh(i).name);

				tempFile.getVertices(i,vertices);
				tempFile.getIndices(i, indices);

				if (vertices.size() && indices.size())
				{
					AddMesh(tempMetaData, vertices, indices);
				}
				else
				{
					FY_CORE_FATAL("Luna Mesh:{0}, has 0 Vertices or Indices ", tempFile.getMesh(i).name);
				}



				//Boundingbox;
				AddBoundingbox(tempMetaData,tempFile.getBoundingBox(tempFile.getMesh(i).id));



				//Animation
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
			FY_CORE_ASSERT("Failed to load file {0}!", FileNameInformation.FileName);
			//FY_CORE_WARN("Luna Failed to load file, FilePath: {0}", FileNameInformation.FileName);
		}
		return returnValue;
	}

	bool AssetManager::LoadGraphicFile(const FileMetaData& FileNameInformation, const bool& Reload)
	{
		bool returnValue = false;

		

		if (AddTexture(FileNameInformation))
		{

			testStr.emplace_back(FileNameInformation.FileName);

			//if (s_AutoLoad)
			//{
			//	//Temp
			//	GetTexture(FileNameInformation.FileName)->LoadToMem();
			//	GetTexture(FileNameInformation.FileName)->LoadToGPU();
			//	GetTexture(FileNameInformation.FileName)->DeleteFromMem();
			//}
			returnValue = true;
		}

		return returnValue;
	}

	bool AssetManager::GetFileInformation(FileMetaData& FileNameInformation)
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

	int8_t AssetManager::GetFileType(const std::string& fileType)
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

	void AssetManager::LoadDir(const std::string& dir)
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

	void AssetManager::ConnectWatchList()
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

	std::string AssetManager::CutFileName(const char* in_char_ptr)
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

	std::string AssetManager::CutFileExtention(const char* in_char_ptr)
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

	std::string AssetManager::CharToStr(const char* in_char_ptr)
	{
		std::string returnString = "";

		if (*in_char_ptr != '\0')
		{

			uint8_t count = 0;
			while (in_char_ptr[count] != '\0')
			{
				returnString.push_back(in_char_ptr[count]);
				count++;
			}
		}
		else
		{
			FY_CORE_WARN("Trying to convert a emtpy char ptr");
		}
		return returnString;
	}

}