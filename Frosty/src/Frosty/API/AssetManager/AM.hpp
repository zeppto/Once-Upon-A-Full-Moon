#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include"AssetFiles/Animation.hpp"
#include"AssetFiles/Mesh.hpp"
#include"AssetFiles/LinkedMaterial.hpp"
#include"KeyLabel.hpp"
#include"BaseKey.hpp"


#define FILE_TYPE_JPG "jpg"
#define FILE_TYPE_PNG "png"
#define FILE_TYPE_LUNA "lu"
#define FILE_TYPE_TTF "ttf"

namespace Frosty
{


	enum FileType
	{
		JPG,
		PNG,
		LUNA,
		TTFF
	};



	class Assetmanager {


	public:		//Variables

	private:	//Variables
		static Assetmanager* s_Instance;
		static uint16_t s_Total_Nr_Assets;

		static uint16_t s_Failed_Loading_Attempts;
		static uint16_t s_Success_Loading_Attempts;

		//Not able to use shared here
		//static std::unordered_map<std::string, std::shared_ptr<AssetFile>>  s_Assets;

		static std::unordered_map<std::string, Mesh> s_Meshes;
		static std::unordered_map<std::string, Animation> s_Animaions;
		static std::unordered_map<std::string, TextureFile> s_Textures;
		static std::unordered_map<std::string, LinkedMaterial> s_LinkedMaterials;

		static std::vector<std::string> s_FilePath_Vector;

		////was not allowed to itterate the unorderedmap
		//static std::vector<std::string> s_MeshNames;
		//static std::vector<std::string> s_AnimationsNames;
		//static std::vector<std::string> s_TexturesNames;
		//static std::vector<std::string> s_MaterialNames;



		friend class Application;

	public:		//Functions

		static Assetmanager* Get();
		~Assetmanager();

		bool LoadFile(const std::string& FullFilePath, const std::string& PreFab_Name = "");
		void LoadFiles();

		void PrintLoadInfo() const;

		static bool LinkKey(const std::string& AssetName, BaseKey* In_Key);

		static bool AddTexture(TextureFile& Tex);
		static bool AddTexture(const FileMetaData& MetaData);

		static bool AddMesh(Mesh& Mesh);
		static bool AddMesh(const FileMetaData& MetaData, const Luna::Mesh& LunMesh);
		
		static bool AddMaterial(LinkedMaterial& LnkMat);
		static bool AddMaterial(const FileMetaData& MetaData, const Luna::Material& LunMat);

		static bool AddAnimation(Animation& Animation);

		//If wanted/needed make declarations in cpp

		inline static Mesh* GetMesh(const std::string& MeshName) { return  MeshLoaded(MeshName) ? &s_Meshes[MeshName] : nullptr; }
		inline static TextureFile* GetTexture(const std::string& FileName) { return  TextureLoaded(FileName) ? &s_Textures[FileName] : nullptr; }
		inline static Animation* GetAnimation(const std::string& AnimationName) { return  AnimationLoaded(AnimationName) ? &s_Animaions[AnimationName] : nullptr; }
		inline static LinkedMaterial* GetMaterial(const std::string& MaterialName) { return  MaterialLoaded(MaterialName) ? &s_LinkedMaterials[MaterialName] : nullptr; }



	private:	//Functions

		inline static void Delete() { if (s_Instance != nullptr) { delete s_Instance; } }

		bool ConnectMaterialWithTexture(LinkedMaterial& Material, const FileMetaData& MetaData);
		bool LoadMaterialTextures(LinkedMaterial*& Material);

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


		const std::string CutFileExtentionFromString(const char* in_char_ptr);



	};







}







#endif // !ASSETMANAGER_H

