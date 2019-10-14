#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include"AssetFiles/Animation.hpp"
#include"AssetFiles/Mesh.hpp"
#include"AssetFiles/LinkedMaterial.hpp"


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

		//was not allowed to itterate the unorderedmap
		static std::vector<std::string> s_MeshNames;
		static std::vector<std::string> s_AnimationsNames;
		static std::vector<std::string> s_TexturesNames;
		static std::vector<std::string> s_MaterialNames;



		friend class Application;

	public:		//Functions

		static Assetmanager* Get();
		~Assetmanager();

		bool LoadFile(const std::string& FullFilePath, const std::string& PreFab_Name = "");
		void LoadFiles();

		void PrintLoadInfo() const;

		inline static bool AddTexture(const FileMetaData& MetaData) { if (TextureLoaded(MetaData.FileName)) { return false; } else { s_Textures[MetaData.FileName] = TextureFile(MetaData); s_TexturesNames.emplace_back(MetaData.FileName); } return true; }
		inline static bool AddMesh(const FileMetaData& MetaData, const Luna::Mesh& LunMesh) { if (MeshLoaded(LunMesh.name)) { return false; } else { s_Meshes[LunMesh.name] = Mesh(MetaData, LunMesh); s_MeshNames.emplace_back(LunMesh.name); } return true; }
		inline static bool AddMaterial(const FileMetaData& MetaData, const std::string& MatName) { if (MaterialLoaded(MatName)) { return false; } else { s_LinkedMaterials[MatName] = LinkedMaterial(MetaData); s_MaterialNames.emplace_back(MatName); } return true; }

		inline static bool AddTexture(TextureFile& Tex) { if (TextureLoaded(Tex.GetfileMetaData().FileName)) { return false; } else { s_Textures[Tex.GetfileMetaData().FileName] = Tex; s_TexturesNames.emplace_back(Tex.GetfileMetaData().FileName); } return true; }
		inline static bool AddMesh(Mesh& Mesh) { if (MeshLoaded(Mesh.GetLunaMesh().name)) { return false; } else { s_Meshes[Mesh.GetLunaMesh().name] = Mesh; s_MeshNames.emplace_back(Mesh.GetLunaMesh().name); } return true; }
		inline static bool AddMaterial(LinkedMaterial& LnkMat) { if (MaterialLoaded(LnkMat.GetfileMetaData().FileName)) { return false; } else { s_LinkedMaterials[LnkMat.GetfileMetaData().FileName] = LnkMat; s_MaterialNames.emplace_back(LnkMat.GetfileMetaData().FileName); } return true; }
		inline static bool AddAnimation(Animation& Animation) { if (AnimationLoaded(Animation.GetName())) { return false; } else { s_Animaions[Animation.GetName()] = Animation; s_AnimationsNames.emplace_back(Animation.GetName()); } return true; }

		inline static Mesh* GetMesh(const std::string& MeshName) { return  MeshLoaded(MeshName) ? &s_Meshes[MeshName] : nullptr; }
		inline static TextureFile* GetTexture(const std::string& FileName) { return  TextureLoaded(FileName) ? &s_Textures[FileName] : nullptr; }
		inline static Animation* GetAnimation(const std::string& AnimationName) { return  AnimationLoaded(AnimationName) ? &s_Animaions[AnimationName] : nullptr; }
		inline static LinkedMaterial* GetMaterial(const std::string& MaterialName) { return  MaterialLoaded(MaterialName) ? &s_LinkedMaterials[MaterialName] : nullptr; }



	private:	//Functions

		inline static void Delete() { if (s_Instance != nullptr) { delete s_Instance; } }

		bool ConnectMaterialWithTexture(LinkedMaterial& Material, const FileMetaData& MetaData);

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

