#ifndef LEVEL_FILE_FORMAT_HPP
#define LEVEL_FILE_FORMAT_HPP

namespace ECS { struct Entity; }

//shuled switch string to const char sizes and maybi glm to flot arrays


//------------How to uppdate old files------------------------
// (intended not tested)
//	1.	create uppdated structs 
//	2.	swap so loaded files use Old_ structs
//	2.	loade old file (coment out in other file what dosent exist)
//	3.	add new fix manualy or on loding 
//	4.	save map (whith new structs)
//------------------------------------------------------------

struct Level_Header
{
	int NrOfEntitys = 0;
	int NrOfComponents = 11;
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
	//nothing to send ( it is emty)
};

//7 = Health
struct Level_Health
{
	float MaxPossibleHealth = 20;						
	float MaxHealth = 5;								
	float CurrentHealth = 5;
};

//8 = HealthBar
struct Level_HealthBar
{
	glm::vec3 BarOffset{ 0.0f, 5.0f, 0.0f };
	char MeshName[50] = "";
	char UseShaderName[50] = "";
	char TextureName[50] = "";

	glm::mat4 HpTransform;
};

//9 = ParticleSystem
struct Level_ParticleSystem
{
	//under construction
};

//10 = LevelExit
struct Level_LevelExit
{
	int ExitDirection = 0;
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

};

struct Level_Entitys
{
	std::vector<Level_Components> myEntitys;
};

//________________________________________________

class LevelFileFormat
{
public:
	LevelFileFormat();
	~LevelFileFormat();

	void AddEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity);
	void SaveToFile(std::string fileName);
	void OpenFromFile(std::string fileName, Frosty::ECS::CTransform* playerTransform = nullptr,
		int rotation = 0, glm::vec3 move = glm::vec3(0,0,0));

private:
	//std::shared_ptr<ECS::Entity> m_ExitEntity;
	Frosty::World* m_World;
	Level_Entitys m_Entitys;
	Level_Header m_Header;
};


#endif