#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include"AssetFiles/Animation.hpp"
#include"AssetFiles/Mesh.hpp"
#include"AssetFiles/LinkedMaterial.hpp"

#include "Frosty/RenderEngine/VertexArray.hpp"
#include "Frosty/RenderEngine/Shader.hpp"
#include "Frosty/RenderEngine/Texture.hpp"
#include "AssetFiles/TrueTypeFile.hpp"
#include"AssetFiles/WeaponHandler.hpp"
#include"AssetFiles/BoolMap.hpp"

#include <Luna/include/Reader.h>

#include"KeyLabel.hpp"
#include"BaseKey.hpp"


#define FILE_TYPE_JPG "jpg"
#define FILE_TYPE_PNG "png"
#define FILE_TYPE_LUNA "lu"
#define FILE_TYPE_TTF "ttf"
#define FILE_TYPE_TGA "tga"
#define FILE_TYPE_XML "xml"
#define FILE_TYPE_GLSL "glsl"
#define FILE_TYPE_BOOLMAP "bmap"

#define MAT_NAME "Mat_" //(followed by a number)
#define MAT_NAME_FOLLOW ":"

namespace Frosty
{


	enum FileType
	{
		JPG,
		PNG,
		LUNA,
		TTF,
		TGA,
		GLSL,
		XML,
		BMAP
	};

	// For storing animation data per vertex!
	struct AnimVert
	{
		float position[3] = { 0.0f };
		float uv[2] = { 0.0f };
		float normal[3] = { 0.0f };
		float tangent[3] = { 0.0f };
		float bitangent[3] = { 0.0f };

		float weights[4] = { 0.0f };
		int joints[4] = { 0 };
	};

	class AssetManager
	{
	public:		//Variables

	private:	//Variables
		static AssetManager* s_Instance;
		static uint16_t s_Total_Nr_Assets;

		static uint16_t s_Failed_Loading_Attempts;
		static uint16_t s_Success_Loading_Attempts;

		static std::unordered_map<std::string, Mesh> s_Meshes;
		static std::unordered_map<std::string, Animation> s_Animations;
		static std::unordered_map<std::string, TextureFile> s_Textures;
		static std::unordered_map<std::string, LinkedMaterial> s_LinkedMaterials;

		//From Temp AssetManager
		static std::map<std::string, std::shared_ptr<VertexArray>> s_VertexArrays;
		static std::map<std::string, std::shared_ptr<Shader>> s_Shaders;
		static std::map<std::string, std::shared_ptr<Texture2D>> s_Textures2D;
		static std::map<std::string, std::shared_ptr<Luna::BoundingBox>> s_BoundingBoxes;
		static std::map<std::string, std::shared_ptr<TrueTypeFile>> s_TruefontTypes;
		static std::map<std::string, std::shared_ptr<WeaponHandler>> s_WeaponHandler;
		static std::map<std::string, std::shared_ptr<BoolMap>> s_BoolMaps;

		static std::unordered_map <std::string, std::list<TextureFile**>> s_TextureWatchList;

		static std::vector<std::string> s_FilePath_Vector;

		static bool s_AutoLoad;

		friend class Application;
		using directory_iterator = std::filesystem::directory_iterator;

	public:		//Functions
		static AssetManager* Get();
		~AssetManager();

		static bool LoadFile(const std::string& FullFilePath, const std::string& TagName = "");
		static void LoadFiles();

		//void PrintLoadInfo();


		//inline static Mesh* GetMesh(const std::string& MeshName) { return  MeshLoaded(MeshName) ? &s_Meshes[MeshName] : nullptr; }
		inline static TextureFile* GetTexture(const std::string& FileName) { return  TextureLoaded(FileName) ? &s_Textures[FileName] : nullptr; }
		inline static Animation* GetAnimation(const std::string& AnimationName) { return  AnimationLoaded(AnimationName) ? &s_Animations[AnimationName] : nullptr; }
		inline static LinkedMaterial* GetMaterial(const std::string& MaterialName) { return  MaterialLoaded(MaterialName) ? &s_LinkedMaterials[MaterialName] : nullptr; }


		inline static std::unordered_map<std::string, Mesh>* GetMeshMap() { return  &s_Meshes; }
		inline static std::unordered_map<std::string, Animation>* GetAnimationMap() { return &s_Animations; }
		inline static std::unordered_map<std::string, TextureFile>* GetTextureMap() { return &s_Textures; }
		inline static std::unordered_map<std::string, LinkedMaterial>* GetMaterialMap() { return &s_LinkedMaterials; }


		//Use Mesh Name
		inline static std::shared_ptr<VertexArray>& GetMesh(const std::string& MeshName) { FY_CORE_ASSERT(s_VertexArrays.count(MeshName), "Mesh error!\n{0} doesn't exist!", MeshName); return s_VertexArrays[MeshName]; }
		inline static std::map<std::string, std::shared_ptr<VertexArray>>& GetMeshes() { return s_VertexArrays; }

		//Use File Name
		inline static std::shared_ptr<Shader>& GetShader(const std::string& FileName) { FY_CORE_ASSERT(s_Shaders.count(FileName), "Shader error!\n{0} doesn't exist!", FileName); return s_Shaders[FileName]; }
		inline static std::shared_ptr<TrueTypeFile>& GetTTF(const std::string& FileName) { FY_CORE_ASSERT(s_TruefontTypes.count(FileName), "TTF error!\n{0} doesn't exist!", FileName); return s_TruefontTypes[FileName]; }
		inline static std::map<std::string, std::shared_ptr<Shader>>& GetShaders() { return s_Shaders; }

		// Textures

		//Use File Name
		inline static std::shared_ptr<Texture2D>& GetTexture2D(const std::string& FileName) { FY_CORE_ASSERT(s_Textures2D.count(FileName), "Texture2D error!\n{0} doesn't exist!", FileName); return s_Textures2D[FileName]; }
		inline static std::map<std::string, std::shared_ptr<Texture2D>>& GetTextures2D() { return s_Textures2D; }

		//Use Mesh Name
		inline static std::shared_ptr<Luna::BoundingBox>& GetBoundingBox(const std::string& MeshName) { FY_CORE_ASSERT(s_BoundingBoxes.count(MeshName), "Texture2D error!\n{0} doesn't exist!", MeshName); return s_BoundingBoxes[MeshName]; }
		inline static std::map<std::string, std::shared_ptr<Luna::BoundingBox>>& GetBoundingBoxes() { return s_BoundingBoxes; }


		//Use File Name
		inline static std::shared_ptr<WeaponHandler>& GetWeaponHandler(const std::string& FileName) { FY_CORE_ASSERT(s_WeaponHandler.count(FileName), "WeaponHandler error!\n{0} doesn't exist!", FileName); return s_WeaponHandler[FileName]; }
		inline static std::map<std::string, std::shared_ptr<WeaponHandler>>& GetWeaponHandlers() { return s_WeaponHandler; }


		//Use File Name
		inline static std::shared_ptr<BoolMap>& GetBoolMap (const std::string& FileName) { FY_CORE_ASSERT(s_BoolMaps.count(FileName), "BoolMap error!\n{0} doesn't exist!", FileName); return s_BoolMaps[FileName]; }
		inline static std::map<std::string, std::shared_ptr<BoolMap>>& GetBoolMaps() { return s_BoolMaps; }

		//Use File Name

		static std::vector<std::string> GetMeshNames();
		static std::vector<std::string> GetShaderNames();
		static std::vector<std::string> GetTexturesNames();
		static std::vector<std::string> GetBoundingBoxNames();

	private:	//Functions

		inline static void Delete() { if (s_Instance != nullptr) { delete s_Instance; } }

		inline static void AddTexPtrToWatchList(const std::string& fileName, TextureFile*& ref) { s_TextureWatchList[fileName].emplace_back(&ref); }

		static bool FileLoaded(const std::string& FilePath);
		static bool AssetLoaded(const std::string& AssetName);

		static bool MaterialLoaded(const std::string& FileName);
		static bool TTFLoaded(const std::string& FileName);
		static bool XMLLoaded(const std::string& FileName);
		static bool TextureLoaded(const std::string& FileName);
		static bool AnimationLoaded(const std::string& AssetName);
		static bool MeshLoaded(const std::string& AssetName);
		static bool BoundingboxLoaded(const std::string& MeshName);
		static bool BmapLoaded(const std::string& FileName);

		static 	bool LoadLunaFile(const FileMetaData& FileNameInformation, const bool& Reload = false);
		static 	bool LoadTTF_File(const FileMetaData& FileNameInformation, const bool& Reload = false);
		static 	bool LoadGraphicFile(const FileMetaData& FileNameInformation, const bool& Reload = false);
		static bool LoadXML(const FileMetaData& FileNameInformation, const bool& Reload = false);
		static bool LoadBmap(const FileMetaData& FileNameInformation, const bool& Reload = false);

		static 	bool GetFileInformation(FileMetaData& FileNameInformation);
		static 	int8_t GetFileType(const std::string& fileType);

		static void LoadDir(const std::string& dir);
		static void ConnectWatchList();


		static std::string CutFileName(const char* in_char_ptr);
		static std::string CutFileExtention(const char* in_char_ptr);
		static std::string CharToStr(const char* in_char_ptr);


		// Meshes
		static bool AddMesh(const FileMetaData& MetaData,const std::vector<Luna::Vertex>& vertices, const std::vector<Luna::Index>& indices);
		//Animation is TEMPORARY!
		static bool AddAnimatedMesh(const FileMetaData& MetaData, const std::vector<AnimVert>& vertices, const std::vector<Luna::Index>& indices, Luna::Animation& temp);

		static bool AddTexture(const FileMetaData& MetaData);
		static bool AddBmap(const FileMetaData& MetaData);
		static bool AddTTF(const FileMetaData& MetaData);
		static bool AddXML(const FileMetaData& MetaData);

		static bool AddMaterial(LinkedMaterial& LnkMat);
		static bool AddMaterial(const FileMetaData& MetaData, const Luna::Material& LunMat);

		static std::vector<AnimVert> MakeAnimVerts(Luna::Reader& tmpFile);
		static bool AddAnimation(Animation& Animation);
		static bool AddBoundingbox(const FileMetaData& MetaData,const Luna::BoundingBox& Boundinbox);


	};
}
#endif // !ASSETMANAGER_H