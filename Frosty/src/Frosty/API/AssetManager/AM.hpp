#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include"AssetFiles/Animation.hpp"
#include"AssetFiles/Mesh.hpp"
#include"AssetFiles/LinkedMaterial.hpp"

#include "Frosty/RenderEngine/VertexArray.hpp"
#include "Frosty/RenderEngine/Shader.hpp"
#include "Frosty/RenderEngine/Texture.hpp"

#include <Luna/include/Reader.h>

#include"KeyLabel.hpp"
#include"BaseKey.hpp"


#define FILE_TYPE_JPG "jpg"
#define FILE_TYPE_PNG "png"
#define FILE_TYPE_LUNA "lu"
#define FILE_TYPE_TTF "ttf"
#define FILE_TYPE_TGA "tga"
#define FILE_TYPE_GLSL "glsl"

#define MAT_NAME "Mat_" //(followed by a number)
#define MAT_NAME_FOLLOW ":"

namespace Frosty
{


	enum FileType
	{
		JPG,
		PNG,
		LUNA,
		TTFF,
		TGA,
		GLSL
	};



	class AssetManager {


	public:		//Variables

	private:	//Variables
		static AssetManager* s_Instance;
		static uint16_t s_Total_Nr_Assets;

		static uint16_t s_Failed_Loading_Attempts;
		static uint16_t s_Success_Loading_Attempts;

		static std::unordered_map<std::string, Mesh> s_Meshes;
		static std::unordered_map<std::string, Animation> s_Animaions;
		static std::unordered_map<std::string, TextureFile> s_Textures;
		static std::unordered_map<std::string, LinkedMaterial> s_LinkedMaterials;

		//From Temp AssetManager
		static std::map<std::string, std::shared_ptr<VertexArray>> s_VertexArrays;
		static std::map<std::string, std::shared_ptr<Shader>> s_Shaders;
		static std::map<std::string, std::shared_ptr<Texture2D>> s_Textures2D;
		static std::map<std::string, std::shared_ptr<Luna::BoundingBox>> s_BoundingBoxes;

		static std::unordered_map <std::string, std::list<TextureFile**>> s_TextureWatchList;

		static std::vector<std::string> s_FilePath_Vector;

		static bool s_AutoLoad;

		friend class Application;
		using directory_iterator = std::filesystem::directory_iterator;

	public:		//Functions

		static AssetManager* Get();
		~AssetManager();

		bool LoadFile(const std::string& FullFilePath, const std::string& TagName = "");
		void LoadFiles();

		void PrintLoadInfo() const;

		//static bool LinkKey(const std::string& AssetName, BaseKey* In_Key);

		static bool AddTexture(TextureFile& Tex);
		static bool AddTexture(const FileMetaData& MetaData);

		//static bool AddMesh(Mesh& Mesh);
		static bool AddMesh(const FileMetaData& MetaData, const Luna::Mesh& LunMesh);
		
		static bool AddMaterial(LinkedMaterial& LnkMat);
		static bool AddMaterial(const FileMetaData& MetaData, const Luna::Material& LunMat);

		static bool AddAnimation(Animation& Animation);

		//inline static Mesh* GetMesh(const std::string& MeshName) { return  MeshLoaded(MeshName) ? &s_Meshes[MeshName] : nullptr; }
		inline static TextureFile* GetTexture(const std::string& FileName) { return  TextureLoaded(FileName) ? &s_Textures[FileName] : nullptr; }
		inline static Animation* GetAnimation(const std::string& AnimationName) { return  AnimationLoaded(AnimationName) ? &s_Animaions[AnimationName] : nullptr; }
		inline static LinkedMaterial* GetMaterial(const std::string& MaterialName) { return  MaterialLoaded(MaterialName) ? &s_LinkedMaterials[MaterialName] : nullptr; }


		inline static std::unordered_map<std::string, Mesh>* GetMeshMap() { return  &s_Meshes; }
		inline static std::unordered_map<std::string, Animation>* GetAnimationMap() { return &s_Animaions; }
		inline static std::unordered_map<std::string, TextureFile>* GetTextureMap() { return &s_Textures; }
		inline static std::unordered_map<std::string, LinkedMaterial>* GetMaterialMap() { return &s_LinkedMaterials; }


		//From Temp AssetManager

		// Meshes
		inline static std::shared_ptr<VertexArray>& GetMesh(const std::string& name) { FY_CORE_ASSERT(s_VertexArrays.count(name), "Mesh error!\n{0} doesn't exist!", name); return s_VertexArrays[name]; }
		inline static std::map<std::string, std::shared_ptr<VertexArray>>& GetMeshes() { return s_VertexArrays; }

		// Shaders
		inline static std::shared_ptr<Shader>& GetShader(const std::string& name) { FY_CORE_ASSERT(s_Shaders.count(name), "Shader error!\n{0} doesn't exist!", name); return s_Shaders[name]; }
		inline static std::map<std::string, std::shared_ptr<Shader>>& GetShaders() { return s_Shaders; }

		// Textures
		inline static std::shared_ptr<Texture2D>& GetTexture2D(const std::string& name) { FY_CORE_ASSERT(s_Textures2D.count(name), "Texture2D error!\n{0} doesn't exist!", name); return s_Textures2D[name]; }
		inline static std::map<std::string, std::shared_ptr<Texture2D>>& GetTextures2D() { return s_Textures2D; }

		// Textures
		inline static std::shared_ptr<Luna::BoundingBox>& GetBoundingBox(const std::string& name) { return s_BoundingBoxes[name]; }
		inline static std::map<std::string, std::shared_ptr<Luna::BoundingBox>>& GetBoundingBoxes() { return s_BoundingBoxes; }






	private:	//Functions

		inline static void Delete() { if (s_Instance != nullptr) { delete s_Instance; } }

		inline static void AddTexPtrToWatchList(const std::string& fileName, TextureFile*& ref) { s_TextureWatchList[fileName].emplace_back(&ref); }

		static bool FileLoaded(const std::string& FilePath);
		static bool AssetLoaded(const std::string& AssetName);

		static bool MaterialLoaded(const std::string& FileName);
		static bool TextureLoaded(const std::string& FileName);
		static bool AnimationLoaded(const std::string& AssetName);
		static bool MeshLoaded(const std::string& AssetName);

		//from ML
		bool LoadLunaFile(const FileMetaData& FileNameInformation, const bool& Reload = false);
		bool LoadGraphicFile(const FileMetaData& FileNameInformation, const bool& Reload = false);
		bool GetFileInformation(FileMetaData& FileNameInformation);
		int8_t GetFileType(const std::string& fileType) const;

		void LoadDir(const std::string& dir);
		void ConnectWatchList();


		const std::string CutFileName(const char* in_char_ptr);
		const std::string CutFileExtention(const char* in_char_ptr);




		//From Temp AssetManager
		// Meshes
		static void AddMesh(const std::string& name, const std::string& filepath);
		// Textures
		static void LoadTexture2D(const std::string& name, const std::string& filepath);


	};







}







#endif // !ASSETMANAGER_H

