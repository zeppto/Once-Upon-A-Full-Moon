#ifndef LEVEL_FILE_FORMAT_HPP
#define LEVEL_FILE_FORMAT_HPP

namespace ECS { struct Entity; }
//namespace ECS { struct BoolMap; }

//shuled switch string to const char sizes and maybi glm to flot arrays


//------------How to uppdate old files------------------------
// (intended not tested)
//	1.	create uppdated structs 
//	2.	swap so loaded files use Old_ structs
//	2.	loade old file (coment out in other file what dosent exist)
//	3.	add new fix manualy or on loding 
//	4.	save map (whith new structs)
//------------------------------------------------------------

namespace MCS
{
	struct Level_Header
	{
		int NrOfEntitys = 0;
		int NrOfComponents = 14;
	};

	//Do this per nrOfEntitys and as meny times as nrOfComponents (to make older files have a higer shans of combadebilety) 
	//if nrOfComponents = 10 go from 0 - 9 for entetys
	struct Level_Component
	{
		bool HaveComponent = false;
	};
	//if haveComponent is true do the coresponding struct

	//0 = Transform
	struct Level_Transform
	{
		glm::vec3 Position{ 0.0f };
		glm::vec3 Rotation{ 0.0f };
		glm::vec3 Scale{ 1.0f };
		bool IsStatic{ false };
	};
	//temp
	struct Level_Transform_Old
	{
		glm::vec3 Position{ 0.0f };
		glm::vec3 Rotation{ 0.0f };
		glm::vec3 Scale{ 1.0f };
	};

	//1 = Mesh
	struct Level_Mesh
	{
		//name for mesh to give to assetManager
		char MeshName[50] = "";
	};

	//2 = Material
	struct Level_Material_old
	{
		char UseShaderName[50] = "";
		glm::vec4 Albedo{ 1.0f, 0.0f, 1.0f, 1.0f };
		char DiffuseTextureName[50] = "";
		char SpecularTextureName[50] = "";
		char NormalTextureName[50] = "";
		char BlendMapTextureName[50] = "";
		char BlendTexture1Name[50] = "";
		char BlendTexture2Name[50] = "";

		float SpecularStrength{ 0.5f };
		int Shininess{ 16 };
		glm::vec2 TextureScale{ 1.0f };
	};
	struct Level_Material
	{
		char UseShaderName[50] = "";
		glm::vec4 Albedo{ 1.0f, 0.0f, 1.0f, 1.0f };
		char DiffuseTextureName[50] = "";
		char SpecularTextureName[50] = "";
		char NormalTextureName[50] = "";
		char BlendMapTextureName[50] = "";
		char BlendTexture1Name[50] = "";
		char BlendTexture2Name[50] = "";

		float SpecularStrength{ 0.5f };
		int Shininess{ 16 };
		glm::vec2 TextureScale{ 1.0f };
		bool HasTransparency = false;
	};

	//3 = Follow
	struct Level_Follow
	{
		//target player if no tarket
		//!!! have not chosen how to do this yet
		float StopDistance{ 0.0f };
	};

	//4 = Light
	struct Level_Light
	{
		int Type = 0;
		glm::vec3 Color{ 1.0f, 0.96f, 0.84f };
		float Radius{ 20.0f };
		float Strength{ 1.0f };
	};

	//5 = Physics
	struct Level_Physics
	{
		glm::vec3 Direction{ 0.0f, 0.0f, 0.0f };
		float MaxSpeed{ 100.f };
		float Speed{ 0.0f };
		glm::vec3 Velocity{ 0.0f };
		float SpeedMultiplier{ 1.0f };
	};

	//6 = Enemy
	struct Level_Enemy
	{
		float RunOnHealth{ 0.0f };
		glm::vec3 SpawnPosition{ 0.0f };
	};

	//7 = Health
	struct Level_Health
	{
		int MaxPossibleHealth = 20;
		int MaxHealth = 5;
		int CurrentHealth = 5;
	};

	//8 = HealthBar
	struct Level_HealthBar
	{
		glm::vec3 BarOffset{ 0.0f, 5.0f, 0.0f };
		char MeshName[50] = "";
		char UseShaderName[50] = "";
		char TextureName[50] = "";

		glm::vec3 Translate;
		glm::vec3 Scale;
	};

	//9 = ParticleSystem
	struct Level_ParticleSystem
	{
		uint32_t MaxParticles{ 1 }; //User's choice of max particles
		float StartParticleSize{ 1.0f };
		float EndParticleSize{ 0.0f };
		float ParticleSize{ 1.0f }; //For a constant size

		glm::vec3 SystemRotation{ 0.0f };

		uint32_t ParticleCount{ 0 };
		glm::vec3 SystemStartColor{ 1.0f };
		glm::vec3 SystemEndColor{ 1.0f };
		glm::vec3 ParticleSystemDirection{ 0.0f, 1.0f, 0.0f };
		glm::vec3 ParticleSystemStartPos{ 0.0f, 0.0f, 0.0f };
		float EmitRate{ 0.1f };
		uint32_t EmitCount{ 1 };
		float Speed{ 1.0f };
		float MinLifetime{ 3.0f };
		float MaxLifetime{ 3.0f };
		float FadeTreshold{ 0.0f };
		float FadeInTreshold{ MaxLifetime };

		float randSpread{ 1.5f };
		glm::vec3 randMainDir{ 0.0f, 1.0f, 0.0f };

		bool RotateOverLifetime{ false };
		bool StaticColor{ true };
		bool RandomLifetimes{ false };
		bool RandomStartPos{ false };
		bool RandomDirection{ false };
		bool AlwaysFaceCamera{ true };

		char TextureName[50] = "";
		char ShaderName[50] = "";
	};

	//10 = LevelExit
	struct Level_LevelExit
	{
		int ExitDirection = 0;
	};

	//11 = DropItem
	struct Level_DropItem
	{
		//int ExitDirection = 0;
	};

	//12 = WitchCircle
	struct Level_WitchCircle
	{
		//emty
	};

	//13 = AnimController
	struct Level_AnimController
	{
		//emty
	};

	//n = name
	//struct Level_
	//{
	//};

	//________________________________________________
	//for file saving
	struct Level_Components
	{
		std::vector<Level_Component> MyComponents;
		Level_Transform myTransform;
		Level_Mesh myMesh;
		Level_Material myMaterial;
		Level_Follow myFollow;
		Level_Light myLight;
		Level_Physics myPhysics;
		Level_Enemy myEnemy;
		Level_Health myHealth;
		Level_HealthBar myHealthBar;
		Level_ParticleSystem myParticleSystem;
		Level_LevelExit myLevelExit;
		Level_DropItem myDropItem;
		Level_WitchCircle myWitchCircle;
		Level_AnimController myAnimController;


	};

	struct Level_Entitys
	{
		std::vector<Level_Components> myEntitys;
	};


	struct Level_rememberedEntitys
	{
		glm::ivec2 myRoomId = { 0, 0 };
		std::vector<int> removeChest;
		std::vector<int> removeWitchCirkel;
		std::vector<glm::vec3> addedBait;
		//not in use
		std::vector<int> existingIDs;
		std::vector<int> killedIds;
		float timePlayerLeft;
	};
	//________________________________________________

	class LevelFileFormat
	{
	public:
		LevelFileFormat();
		~LevelFileFormat();

		void AddEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity);
		void SaveToFile(std::string fileName);
		void OpenFromFile(
			std::string fileName,
			const bool& CurrentGroup,
			glm::ivec2 roomId = glm::ivec2(0, 0)
			, Frosty::ECS::CTransform* playerTransform = nullptr,
			int rotation = 0,
			glm::vec3 move = glm::vec3(0, 0, 0),
			const int& RoomExitDir = -1);
		void LoadBoolMap(std::string fileName);
		int NumberOfRoomsVisited();
		bool AddBaitToMap(glm::vec3 baitPos, glm::ivec2 room);
		//std::vector<glm::vec3> GetAllBaitInRoom();
		//int GetNumberOfBaitInRoom();
		int RemoveAllBaitInRoom(glm::ivec2 room);
		void clearVisitedRooms();

	private:
		void AddMaterial(const std::shared_ptr<Frosty::ECS::Entity>& entity, const Level_Entitys& fileEntities, const size_t index);

	private:
		//std::shared_ptr<ECS::Entity> m_ExitEntity;
		Frosty::World* m_World;
		Level_Entitys m_Entitys;
		Level_Header m_Header;
		std::vector<Level_rememberedEntitys> m_VisitedRooms;
	};
}

#endif