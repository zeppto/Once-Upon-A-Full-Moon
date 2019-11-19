#include <mcspch.hpp>
//#include "Frosty.h"
#include "LevelFileFormat.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include <glm/gtx/matrix_decompose.hpp>


LevelFileFormat::LevelFileFormat()
{
	m_World = Frosty::Application::Get().GetWorld().get();
}

LevelFileFormat::~LevelFileFormat()
{
}

void LevelFileFormat::AddEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	if (m_World->HasComponent<Frosty::ECS::CPlayer>(entity) || m_World->HasComponent<Frosty::ECS::CCamera>(entity))
	{
	}
	else
	{
		Level_Components myComponents;
		myComponents.MyComponents.resize(m_Header.NrOfComponents);
		m_Header.NrOfEntitys++;
		//Transform
		if (m_World->HasComponent<Frosty::ECS::CTransform>(entity))
		{
			myComponents.MyComponents.at(0).HaveComponent = true;
			auto& tranform = m_World->GetComponent<Frosty::ECS::CTransform>(entity);
			myComponents.myTransform.Position = tranform.Position;
			myComponents.myTransform.Rotation = tranform.Rotation;
			myComponents.myTransform.Scale = tranform.Scale;
			myComponents.myTransform.IsStatic = tranform.IsStatic;
		}
		else
			myComponents.MyComponents.at(0).HaveComponent = false;
		//Mesh
		if (m_World->HasComponent<Frosty::ECS::CMesh>(entity))
		{
			myComponents.MyComponents.at(1).HaveComponent = true;
			auto& mesh = m_World->GetComponent<Frosty::ECS::CMesh>(entity);
			strcpy_s(myComponents.myMesh.MeshName, mesh.Mesh->GetName().c_str());
		}
		else
			myComponents.MyComponents.at(1).HaveComponent = false;
		//Material
		if (m_World->HasComponent<Frosty::ECS::CMaterial>(entity))
		{
			myComponents.MyComponents.at(2).HaveComponent = true;
			auto& material = m_World->GetComponent<Frosty::ECS::CMaterial>(entity);
			strcpy_s(myComponents.myMaterial.UseShaderName, material.UseShader->GetName().c_str());
			myComponents.myMaterial.Albedo = material.Albedo;
			if(material.DiffuseTexture != nullptr)
				strcpy_s(myComponents.myMaterial.DiffuseTextureName, material.DiffuseTexture->GetName().c_str());
			if (material.SpecularTexture != nullptr)
				strcpy_s(myComponents.myMaterial.SpecularTextureName, material.SpecularTexture->GetName().c_str());
			if (material.NormalTexture != nullptr)
				strcpy_s(myComponents.myMaterial.NormalTextureName, material.NormalTexture->GetName().c_str());
			if (material.BlendMapTexture != nullptr)
				strcpy_s(myComponents.myMaterial.BlendMapTextureName, material.BlendMapTexture->GetName().c_str());
			if (material.BlendTexture1 != nullptr)
				strcpy_s(myComponents.myMaterial.BlendTexture1Name, material.BlendTexture1->GetName().c_str());
			if (material.BlendTexture2 != nullptr)
				strcpy_s(myComponents.myMaterial.BlendTexture2Name, material.BlendTexture2->GetName().c_str());
			myComponents.myMaterial.SpecularStrength = material.SpecularStrength;
			myComponents.myMaterial.Shininess = material.Shininess;
			myComponents.myMaterial.TextureScale = material.TextureScale;
		}
		else
			myComponents.MyComponents.at(2).HaveComponent = false;
		//Follow
		if (m_World->HasComponent<Frosty::ECS::CFollow>(entity))
		{
			myComponents.MyComponents.at(3).HaveComponent = true;
			//under construction
			auto& follow = m_World->GetComponent<Frosty::ECS::CFollow>(entity);
			myComponents.myFollow.StopDistance = follow.StopDistance;
		}
		else
			myComponents.MyComponents.at(3).HaveComponent = false;
		//Light
		if (m_World->HasComponent<Frosty::ECS::CLight>(entity))
		{
			myComponents.MyComponents.at(4).HaveComponent = true;
			auto& light = m_World->GetComponent<Frosty::ECS::CLight>(entity);
			myComponents.myLight.Type = (int)light.Type;
			myComponents.myLight.Color = light.Color;
			myComponents.myLight.Radius = light.Radius;
			myComponents.myLight.Strength = light.Strength;
		}
		else
			myComponents.MyComponents.at(4).HaveComponent = false;
		//Physics
		if (m_World->HasComponent<Frosty::ECS::CPhysics>(entity))
		{
			myComponents.MyComponents.at(5).HaveComponent = true;
			auto& physics = m_World->GetComponent<Frosty::ECS::CPhysics>(entity);

			myComponents.myPhysics.Direction = physics.Direction;
			myComponents.myPhysics.MaxSpeed = physics.MaxSpeed;
			myComponents.myPhysics.Speed = physics.Speed;
			myComponents.myPhysics.Velocity = physics.Velocity;
			myComponents.myPhysics.SpeedMultiplier = physics.SpeedMultiplier;

		}
		else
			myComponents.MyComponents.at(5).HaveComponent = false;
		//Enemy
		if (m_World->HasComponent<Frosty::ECS::CEnemy>(entity))
		{
			myComponents.MyComponents.at(6).HaveComponent = true;
		}
		else
			myComponents.MyComponents.at(6).HaveComponent = false;
		//Health
		if (m_World->HasComponent<Frosty::ECS::CHealth>(entity))
		{
			myComponents.MyComponents.at(7).HaveComponent = true;
			auto& health = m_World->GetComponent<Frosty::ECS::CHealth>(entity);
			myComponents.myHealth.CurrentHealth = health.CurrentHealth;
			myComponents.myHealth.MaxHealth = health.MaxHealth;
			myComponents.myHealth.MaxPossibleHealth = health.MaxPossibleHealth;
		}
		else
			myComponents.MyComponents.at(7).HaveComponent = false;
		//HealthBar
		if (m_World->HasComponent<Frosty::ECS::CHealthBar>(entity))
		{
			myComponents.MyComponents.at(8).HaveComponent = true;
			auto& healthBar = m_World->GetComponent<Frosty::ECS::CHealthBar>(entity);
			myComponents.myHealthBar.BarOffset = healthBar.BarOffset;

			myComponents.myHealthBar.Translate= healthBar.Translate;
			myComponents.myHealthBar.Scale= healthBar.Scale;

			strcpy_s(myComponents.myHealthBar.MeshName, healthBar.Mesh->GetName().c_str());
			strcpy_s(myComponents.myHealthBar.TextureName, healthBar.Texture->GetName().c_str());
			strcpy_s(myComponents.myHealthBar.UseShaderName, healthBar.UseShader->GetName().c_str());
		}
		else
			myComponents.MyComponents.at(8).HaveComponent = false;
		//ParticleSystem
		if (m_World->HasComponent<Frosty::ECS::CParticleSystem>(entity))
		{
			myComponents.MyComponents.at(9).HaveComponent = true;
			//auto& particelSystem = m_World->GetComponent<Frosty::ECS::CParticleSystem>(entity);
			//myComponents.myParticleSystem.startParticleSize = particelSystem.startParticleSize;
			//myComponents.myParticleSystem.endParticleSize = particelSystem.endParticleSize;
			//myComponents.myParticleSystem.particleSystemColor = particelSystem.particleSystemColor;
			//myComponents.myParticleSystem.emitRate = particelSystem.emitRate;
			//myComponents.myParticleSystem.emitCount = particelSystem.emitCount;
			//myComponents.myParticleSystem.maxLifetime = particelSystem.maxLifetime;
			//myComponents.myParticleSystem.fadeTreshold = particelSystem.fadeTreshold;
		}
		else
			myComponents.MyComponents.at(9).HaveComponent = false;
		//LevelExit
		if (m_World->HasComponent<Frosty::ECS::CLevelExit>(entity))
		{
			myComponents.MyComponents.at(10).HaveComponent = true;
			auto& levelExit = m_World->GetComponent<Frosty::ECS::CLevelExit>(entity);
			myComponents.myLevelExit.ExitDirection = levelExit.ExitDirection;
		}
		else
			myComponents.MyComponents.at(10).HaveComponent = false;
		m_Entitys.myEntitys.push_back(myComponents);
	}
}

void LevelFileFormat::SaveToFile(std::string fileName)
{
	int x = 0;
	std::string filePath;
	std::ifstream existingFile;
	do 
	{
		x++;
		filePath = "../../../assets/levels/" + fileName + "_" + std::to_string(x) + ".lvl";
		existingFile.open(filePath);
		existingFile.close();
	} while (existingFile.good());
	//write to file
	std::ofstream myFile;
	myFile.open(filePath, std::ios::binary);
	myFile.write((const char*)&m_Header, sizeof(Level_Header));
	for (int i = 0; i < m_Entitys.myEntitys.size(); i++)
	{
		for(int j = 0; j < m_Header.NrOfComponents; j++)
			myFile.write((const char*)& m_Entitys.myEntitys.at(i).MyComponents.at(j).HaveComponent, sizeof(bool));
		for (int j = 0; j < m_Header.NrOfComponents; j++)
			if (m_Entitys.myEntitys.at(i).MyComponents.at(j).HaveComponent)
			{
				switch (j)
				{
					//0 = Transform
				case 0:
					myFile.write((const char*)& m_Entitys.myEntitys.at(i).myTransform, sizeof(Level_Transform));
					break;
					//1 = Mesh
				case 1:
					myFile.write((const char*)& m_Entitys.myEntitys.at(i).myMesh, sizeof(Level_Mesh));
					break;
					//2 = Material
				case 2:
					myFile.write((const char*)& m_Entitys.myEntitys.at(i).myMaterial, sizeof(Level_Material));
					break;
					//3 = Follow
				case 3:
					myFile.write((const char*)& m_Entitys.myEntitys.at(i).myFollow, sizeof(Level_Follow));
					break;
					//4 = Light
				case 4:
					myFile.write((const char*)& m_Entitys.myEntitys.at(i).myLight, sizeof(Level_Light));
					break;
					//5 = Physics
				case 5:
					myFile.write((const char*)& m_Entitys.myEntitys.at(i).myPhysics, sizeof(Level_Physics));
					break;
					//6 = Enemy
				case 6:
					myFile.write((const char*)& m_Entitys.myEntitys.at(i).myEnemy, sizeof(Level_Enemy));
					break;
					//7 = Health
				case 7:
					myFile.write((const char*)& m_Entitys.myEntitys.at(i).myHealth, sizeof(Level_Health));
					break;
					//8 = HealthBar
				case 8:
					myFile.write((const char*)& m_Entitys.myEntitys.at(i).myHealthBar, sizeof(Level_HealthBar));
					break;
					//9 = ParticleSystem
				case 9:
					myFile.write((const char*)& m_Entitys.myEntitys.at(i).myParticleSystem, sizeof(Level_ParticleSystem));
					break;
					//10 = LevelExit
				case 10:
					myFile.write((const char*)& m_Entitys.myEntitys.at(i).myLevelExit, sizeof(Level_LevelExit));
					break;
				default:
					break;
				}
			}
	}
	myFile.close();
	m_Entitys.myEntitys.clear();


}

void LevelFileFormat::OpenFromFile(std::string fileName, glm::ivec2 roomId , Frosty::ECS::CTransform* playerTransform, int rotation, glm::vec3 move)
{
	std::ifstream existingFile;
	existingFile.open("../../../assets/levels/" + fileName + ".lvl", std::ios::binary);
	Level_Header testHeder;
	Level_Entitys fileEntitys;
	std::vector<std::shared_ptr<Frosty::ECS::Entity>> m_Enemys;
	

	if (existingFile.good())
	{
		existingFile.read((char*)& testHeder, sizeof(Level_Header));
		fileEntitys.myEntitys.resize(testHeder.NrOfEntitys);
		for (int i = 0; i < testHeder.NrOfEntitys; i++)
		{
			//to Fix my problen
			//if (i < 735)
			//{
			//	fileEntitys.myEntitys.at(i).MyComponents.resize(testHeder.NrOfComponents);
			//	for (int j = 0; j < m_Header.NrOfComponents; j++)
			//		existingFile.read((char*)& fileEntitys.myEntitys.at(i).MyComponents.at(j).HaveComponent, sizeof(bool));
			//	for (int j = 0; j < testHeder.NrOfComponents; j++)
			//	{
			//		if (fileEntitys.myEntitys.at(i).MyComponents.at(j).HaveComponent)
			//		{

			//			if (j == 0)
			//				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myTransform, sizeof(Level_Transform));
			//			if (j == 1)
			//				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myMesh, sizeof(Level_Mesh));
			//			if (j == 2)
			//				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myMaterial, sizeof(Level_Material));
			//			if (j == 3)
			//				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myFollow, sizeof(Level_Follow));
			//			if (j == 4)
			//				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myLight, sizeof(Level_Light));
			//			if (j == 5)
			//				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myPhysics, sizeof(Level_Physics));
			//			if (j == 6)
			//				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myEnemy, sizeof(Level_Enemy));
			//			if (j == 7)
			//				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myHealth, sizeof(Level_Health));
			//			if (j == 8)
			//				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myHealthBar, sizeof(Level_HealthBar));
			//			if (j == 9)
			//				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myParticleSystem, sizeof(Level_ParticleSystem));
			//			if (j == 10)
			//				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myLevelExit, sizeof(Level_LevelExit));
			//		}
			//	}
			//}

			fileEntitys.myEntitys.at(i).MyComponents.resize(testHeder.NrOfComponents);
			for (int j = 0; j < m_Header.NrOfComponents; j++)
				existingFile.read((char*)& fileEntitys.myEntitys.at(i).MyComponents.at(j).HaveComponent, sizeof(bool));

			//create entity
			auto& entity = m_World->CreateEntity();

			//0 = Transform
			if (fileEntitys.myEntitys.at(i).MyComponents.at(0).HaveComponent)
			{
				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myTransform, sizeof(Level_Transform));
				auto& tranform = m_World->GetComponent<Frosty::ECS::CTransform>(entity);
				tranform.Position = fileEntitys.myEntitys.at(i).myTransform.Position;
				tranform.Scale = fileEntitys.myEntitys.at(i).myTransform.Scale;
				tranform.Rotation = fileEntitys.myEntitys.at(i).myTransform.Rotation;
				glm::mat4 matrix(1.0f);
				//matrix = glm::rotate(matrix, glm::degrees(90.0f), glm::vec3(0, 1, 0));
				matrix = glm::rotate(matrix, glm::radians((float)rotation), glm::vec3(0, 1, 0));
				matrix = glm::translate(matrix, tranform.Position);
				//matrix = glm::rotate(matrix, tranform.Rotation.x, glm::vec3(1, 0, 0));
				matrix = glm::rotate(matrix, tranform.Rotation.y, glm::vec3(0, 1, 0));
				//matrix = glm::rotate(matrix, tranform.Rotation.z, glm::vec3(0, 0, 1));
				//matrix = glm::scale(matrix, tranform.Scale);
				//temp
				if (rotation == 90 || rotation == 270)
				{
					if (fileEntitys.myEntitys.at(i).MyComponents.at(10).HaveComponent || !fileEntitys.myEntitys.at(i).MyComponents.at(1).HaveComponent)
					{
						tranform.Scale.x = fileEntitys.myEntitys.at(i).myTransform.Scale.z;
						tranform.Scale.z = fileEntitys.myEntitys.at(i).myTransform.Scale.x;
					}
					else
					{
						tranform.Rotation.y += rotation;
					}
				}
				tranform.Position = glm::vec3(matrix[3].x, matrix[3].y, matrix[3].z);
				glm::quat tempRot;
				glm::vec3 skew;
				glm::vec4 perspective;
				//glm::decompose(matrix, tranform.Scale, tempRot, tranform.Position, skew, perspective);
			}
			//1 = Mesh
			if (fileEntitys.myEntitys.at(i).MyComponents.at(1).HaveComponent)
			{
				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myMesh, sizeof(Level_Mesh));
				//for in game
				//if(!fileEntitys.myEntitys.at(i).MyComponents.at(10).HaveComponent)
				m_World->AddComponent<Frosty::ECS::CMesh>(entity,
					Frosty::AssetManager::GetMesh(fileEntitys.myEntitys.at(i).myMesh.MeshName));
			}
			//2 = Material
			if (fileEntitys.myEntitys.at(i).MyComponents.at(2).HaveComponent)
			{
				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myMaterial, sizeof(Level_Material));
				auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(entity,
					Frosty::AssetManager::GetShader(fileEntitys.myEntitys.at(i).myMaterial.UseShaderName));
				material.Albedo = fileEntitys.myEntitys.at(i).myMaterial.Albedo;
				if ((std::string)fileEntitys.myEntitys.at(i).myMaterial.DiffuseTextureName != "")
					material.DiffuseTexture = Frosty::AssetManager::GetTexture2D(fileEntitys.myEntitys.at(i).myMaterial.DiffuseTextureName);
				if ((std::string)fileEntitys.myEntitys.at(i).myMaterial.SpecularTextureName != "")
					material.SpecularTexture = Frosty::AssetManager::GetTexture2D(fileEntitys.myEntitys.at(i).myMaterial.SpecularTextureName);
				if ((std::string)fileEntitys.myEntitys.at(i).myMaterial.NormalTextureName != "")
					material.NormalTexture = Frosty::AssetManager::GetTexture2D(fileEntitys.myEntitys.at(i).myMaterial.NormalTextureName);
				if ((std::string)fileEntitys.myEntitys.at(i).myMaterial.BlendMapTextureName != "")
					material.BlendMapTexture = Frosty::AssetManager::GetTexture2D(fileEntitys.myEntitys.at(i).myMaterial.BlendMapTextureName);
				if ((std::string)fileEntitys.myEntitys.at(i).myMaterial.BlendTexture1Name != "")
					material.BlendTexture1 = Frosty::AssetManager::GetTexture2D(fileEntitys.myEntitys.at(i).myMaterial.BlendTexture1Name);
				if ((std::string)fileEntitys.myEntitys.at(i).myMaterial.BlendTexture2Name != "")
					material.BlendTexture2 = Frosty::AssetManager::GetTexture2D(fileEntitys.myEntitys.at(i).myMaterial.BlendTexture2Name);
				material.SpecularStrength = fileEntitys.myEntitys.at(i).myMaterial.SpecularStrength;
				material.Shininess = fileEntitys.myEntitys.at(i).myMaterial.Shininess;
				material.TextureScale = fileEntitys.myEntitys.at(i).myMaterial.TextureScale;
			}
			//3 = Follow
			if (fileEntitys.myEntitys.at(i).MyComponents.at(3).HaveComponent)
			{
				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myFollow, sizeof(Level_Follow));
				auto& follow = m_World->AddComponent<Frosty::ECS::CFollow>(entity, playerTransform);
				//For edeting old level
				//auto& follow = m_World->AddComponent<Frosty::ECS::CFollow>(entity);// , playerTransform);
				follow.StopDistance = fileEntitys.myEntitys.at(i).myFollow.StopDistance;
				m_Enemys.push_back(entity);
				//under construction
			}
			//4 = Light
			if (fileEntitys.myEntitys.at(i).MyComponents.at(4).HaveComponent)
			{
				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myLight, sizeof(Level_Light));
				auto& light = m_World->AddComponent<Frosty::ECS::CLight>(entity);
				light.Color = fileEntitys.myEntitys.at(i).myLight.Color;
				light.Radius = fileEntitys.myEntitys.at(i).myLight.Radius;
				light.Strength = fileEntitys.myEntitys.at(i).myLight.Strength;
				light.Type = (Frosty::ECS::CLight::LightType)fileEntitys.myEntitys.at(i).myLight.Type;
			}
			//5 = Physics
			if (fileEntitys.myEntitys.at(i).MyComponents.at(5).HaveComponent)
			{
				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myPhysics, sizeof(Level_Physics));
				auto& physics = m_World->AddComponent<Frosty::ECS::CPhysics>(entity);
				if (fileEntitys.myEntitys.at(i).MyComponents.at(1).HaveComponent)
					physics.BoundingBox = Frosty::AssetManager::GetBoundingBox(fileEntitys.myEntitys.at(i).myMesh.MeshName);
				else
					physics.BoundingBox = Frosty::AssetManager::GetBoundingBox("pCube1");
				physics.Direction = fileEntitys.myEntitys.at(i).myPhysics.Direction;
				physics.MaxSpeed = fileEntitys.myEntitys.at(i).myPhysics.MaxSpeed;
				physics.Speed = fileEntitys.myEntitys.at(i).myPhysics.Speed;
				physics.SpeedMultiplier = fileEntitys.myEntitys.at(i).myPhysics.SpeedMultiplier;
				physics.Velocity = fileEntitys.myEntitys.at(i).myPhysics.Velocity;
			}
			//6 = Enemy
			if (fileEntitys.myEntitys.at(i).MyComponents.at(6).HaveComponent)
			{
				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myEnemy, sizeof(Level_Enemy));
				m_World->AddComponent<Frosty::ECS::CEnemy>(entity, playerTransform);
			}
			//7 = Health
			if (fileEntitys.myEntitys.at(i).MyComponents.at(7).HaveComponent)
			{
				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myHealth, sizeof(Level_Health));
				auto& health = m_World->AddComponent<Frosty::ECS::CHealth>(entity);
				health.CurrentHealth = fileEntitys.myEntitys.at(i).myHealth.CurrentHealth;
				health.MaxHealth = fileEntitys.myEntitys.at(i).myHealth.MaxHealth;
				health.MaxPossibleHealth = fileEntitys.myEntitys.at(i).myHealth.MaxPossibleHealth;
			}
			//8 = HealthBar
			if (fileEntitys.myEntitys.at(i).MyComponents.at(8).HaveComponent)
			{
				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myHealthBar, sizeof(Level_HealthBar));
				auto& healthBar = m_World->AddComponent<Frosty::ECS::CHealthBar>(entity);
				healthBar.BarOffset = fileEntitys.myEntitys.at(i).myHealthBar.BarOffset;
				//healthBar.hpTransform = fileEntitys.myEntitys.at(i).myHealthBar.HpTransform;
				healthBar.Mesh = Frosty::AssetManager::GetMesh(fileEntitys.myEntitys.at(i).myHealthBar.MeshName);
				healthBar.Texture = Frosty::AssetManager::GetTexture2D(fileEntitys.myEntitys.at(i).myHealthBar.TextureName);
				healthBar.UseShader = Frosty::AssetManager::GetShader(fileEntitys.myEntitys.at(i).myHealthBar.UseShaderName);
			}
			//9 = ParticleSystem
			if (fileEntitys.myEntitys.at(i).MyComponents.at(9).HaveComponent)
			{
				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myParticleSystem, sizeof(Level_ParticleSystem));
				//not in use for now
			}
			//10 = LevelExit
			if (fileEntitys.myEntitys.at(i).MyComponents.at(10).HaveComponent)
			{
				existingFile.read((char*)& fileEntitys.myEntitys.at(i).myLevelExit, sizeof(Level_LevelExit));
				int newExit = fileEntitys.myEntitys.at(i).myLevelExit.ExitDirection;
				if (rotation == 270)
				{
					if (fileEntitys.myEntitys.at(i).myLevelExit.ExitDirection == 0)
						newExit = 3;
					if (fileEntitys.myEntitys.at(i).myLevelExit.ExitDirection == 3)
						newExit = 1;
					if (fileEntitys.myEntitys.at(i).myLevelExit.ExitDirection == 1)
						newExit = 2;
					if (fileEntitys.myEntitys.at(i).myLevelExit.ExitDirection == 2)
						newExit = 0;
				}
				else if (rotation == 180)
				{
					if (fileEntitys.myEntitys.at(i).myLevelExit.ExitDirection == 0)
						newExit = 1;
					if (fileEntitys.myEntitys.at(i).myLevelExit.ExitDirection == 2)
						newExit = 3;
					if (fileEntitys.myEntitys.at(i).myLevelExit.ExitDirection == 1)
						newExit = 0;
					if (fileEntitys.myEntitys.at(i).myLevelExit.ExitDirection == 3)
						newExit = 2;
				}
				else if (rotation == 90)
				{
					if (fileEntitys.myEntitys.at(i).myLevelExit.ExitDirection == 0)
						newExit = 2;
					if (fileEntitys.myEntitys.at(i).myLevelExit.ExitDirection == 2)
						newExit = 1;
					if (fileEntitys.myEntitys.at(i).myLevelExit.ExitDirection == 1)
						newExit = 3;
					if (fileEntitys.myEntitys.at(i).myLevelExit.ExitDirection == 3)
						newExit = 0;
				}
				m_World->AddComponent<Frosty::ECS::CLevelExit>(entity, newExit);
			}

		}
		
		//to remove "enemys" or chest m.m to control the number and randomize pos
		int enteredRoomId = -1;
		int nrToHave = 2;
		for (int i = 0; i < m_VisitedRooms.size(); i++)
		{
			if (m_VisitedRooms.at(i).myRoomId == roomId)
			{
				enteredRoomId = i;
			}
		}
		//if (enteredRoomId != -1)
		//{
		//	if (m_Enemys.size() >= nrToHave)
		//	{
		//		for (int i = 0; i < m_Enemys.size() - nrToHave; i++)
		//		{
		//			if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Enemys.at(m_VisitedRooms.at(enteredRoomId).removeEnemy.at(i))))
		//			{
		//				m_World->AddComponent<Frosty::ECS::CDestroy>(m_Enemys.at(m_VisitedRooms.at(enteredRoomId).removeEnemy.at(i)));
		//			}
		//			std::shared_ptr<Frosty::ECS::Entity> temp = m_Enemys.at(m_VisitedRooms.at(enteredRoomId).removeEnemy.at(i));
		//			m_Enemys.at(m_VisitedRooms.at(enteredRoomId).removeEnemy.at(i)) = m_Enemys.at(m_Enemys.size() - 1 - i);
		//			m_Enemys.back() = temp;
		//		}
		//	}
		//}
		//else
		//{
		//	Level_rememberedEntitys rEntitys;
		//	rEntitys.myRoomId = roomId;
		//	//temp nr of 
		//	if (m_Enemys.size() >= nrToHave)
		//	{
		//		for (int i = 0; i < m_Enemys.size() - nrToHave; i++)
		//		{
		//			int rnd = rand() % (m_Enemys.size() - i);
		//			if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Enemys.at(rnd)))
		//			{
		//				rEntitys.removeEnemy.push_back(rnd);
		//				m_World->AddComponent<Frosty::ECS::CDestroy>(m_Enemys.at(rnd));
		//			}
		//			std::shared_ptr<Frosty::ECS::Entity> temp = m_Enemys.at(rnd);
		//			m_Enemys.at(rnd) = m_Enemys.at(m_Enemys.size() - 1 - i);
		//			m_Enemys.back() = temp;
		//		}
		//	}
		//	m_VisitedRooms.push_back(rEntitys);
		//}
	}
	existingFile.close();
}



