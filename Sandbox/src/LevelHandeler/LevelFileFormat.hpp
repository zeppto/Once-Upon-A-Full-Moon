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
	bool HaveComponent;
};
//if haveComponent is true do the coresponding struct

//0 = Transform
struct Level_Transform
{
	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scale;
	glm::mat4 ModelMatrix;
};

//1 = Mesh
struct Level_Mesh
{
	//name for mesh to give to assetManager
	std::string MeshName;
};

//2 = Material
struct Level_Material
{
	std::string UseShaderName;
	glm::vec4 Albedo;
	std::string DiffuseTextureName;
	std::string SpecularTextureName;
	std::string NormalTextureName;
	std::string BlendMapTextureName;
	std::string BlendTexture1Name;
	std::string BlendTexture2Name;

	float SpecularStrength;
	int Shininess;
	glm::vec2 TextureScale;
};

//3 = Follow
struct Level_Follow
{
	//target player if no tarket
	//!!! have not chosen how to do this yet
	float StopDistance;
};

//4 = Light
struct Level_Light
{
	int Type;
	glm::vec3 Color;
	float Radius;
	float Strength;
};

//5 = Physics
struct Level_Physics
{
	Luna::BoundingBox BoundingBox;
	glm::vec3 Direction;
	float MaxSpeed;							
	float Speed;
	glm::vec3 Velocity;
	float SpeedMultiplier;
};

//6 = Enemy
struct Level_Enemy
{
	//nothing to send ( it is emty)
};

//7 = Health
struct Level_Health
{
	float MaxPossibleHealth;						
	float MaxHealth;								
	float CurrentHealth;
};

//8 = HealthBar
struct Level_HealthBar
{
	glm::vec3 BarOffset;
	std::string MeshName;
	std::string UseShaderName;
	std::string TextureName;

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
	int ExitDirection;
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
	void SaveToFile();

private:
	//std::shared_ptr<ECS::Entity> m_ExitEntity;
	Frosty::World* m_World;
	Level_Entitys m_Entitys;
	Level_Header m_Header;
};


#endif