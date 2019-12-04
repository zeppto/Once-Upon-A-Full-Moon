#include <mcspch.hpp>
//#include "Frosty.h"
#include "LevelFileFormat.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include <glm/gtx/matrix_decompose.hpp>
#include "Frosty/Core/BoolMap/BoolMapGenerator.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{

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
				std::string meskhh = mesh.Mesh->GetName().c_str();
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
				if (material.DiffuseTexture != nullptr)
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
				std::string meshName = myComponents.myMesh.MeshName;
				myComponents.myMaterial.HasTransparency = material.HasTransparency;

				/*if (m_World->HasComponent<Frosty::ECS::CMesh>(entity) && meshName.find("tree") != std::string::npos)
					myComponents.myMaterial.HasTransparency = true;
				if (m_World->HasComponent<Frosty::ECS::CMesh>(entity) && meshName.find("Tree") != std::string::npos)
					myComponents.myMaterial.HasTransparency = true;*/
			}
			else
				myComponents.MyComponents.at(2).HaveComponent = false;
			//Follow
			//if (m_World->HasComponent<Frosty::ECS::CFollow>(entity))
			//{
			//	myComponents.MyComponents.at(3).HaveComponent = true;
			//	//under construction
			//	auto& follow = m_World->GetComponent<Frosty::ECS::CFollow>(entity);
			//	myComponents.myFollow.StopDistance = follow.StopDistance;
			//}
			//else
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
				//myComponents.myPhysics.Velocity = physics.Velocity;
				myComponents.myPhysics.SpeedMultiplier = physics.SpeedMultiplier;
			}
			else
				myComponents.MyComponents.at(5).HaveComponent = false;
			//Enemy
			if (m_World->HasComponent<Frosty::ECS::CEnemy>(entity))
			{
				myComponents.MyComponents.at(6).HaveComponent = true;
				auto& enemy = m_World->GetComponent<Frosty::ECS::CEnemy>(entity);
				myComponents.myEnemy.RunOnHealth = enemy.RunOnHealth;
				myComponents.myEnemy.SpawnPosition = enemy.SpawnPosition;
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

				myComponents.myHealthBar.Translate = healthBar.Translate;
				myComponents.myHealthBar.Scale = healthBar.Scale;

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
				auto& particelSystem = m_World->GetComponent<Frosty::ECS::CParticleSystem>(entity);
				myComponents.myParticleSystem.AlwaysFaceCamera = particelSystem.AlwaysFaceCamera;
				myComponents.myParticleSystem.EmitCount = particelSystem.EmitCount;
				myComponents.myParticleSystem.EmitRate = particelSystem.EmitRate;
				myComponents.myParticleSystem.EndParticleSize = particelSystem.EndParticleSize;
				myComponents.myParticleSystem.FadeInTreshold = particelSystem.FadeInTreshold;
				myComponents.myParticleSystem.FadeTreshold = particelSystem.FadeTreshold;
				myComponents.myParticleSystem.MaxLifetime = particelSystem.MaxLifetime;
				myComponents.myParticleSystem.MaxParticles = particelSystem.MaxParticles;
				myComponents.myParticleSystem.MinLifetime = particelSystem.MinLifetime;
				myComponents.myParticleSystem.ParticleCount = particelSystem.ParticleCount;
				myComponents.myParticleSystem.ParticleSize = particelSystem.ParticleSize;
				myComponents.myParticleSystem.ParticleSystemDirection = particelSystem.ParticleSystemDirection;
				myComponents.myParticleSystem.ParticleSystemStartPos = particelSystem.ParticleSystemStartPos;
				myComponents.myParticleSystem.randMainDir = particelSystem.randMainDir;
				myComponents.myParticleSystem.RandomDirection = particelSystem.RandomDirection;
				myComponents.myParticleSystem.RandomLifetimes = particelSystem.RandomLifetimes;
				myComponents.myParticleSystem.RandomStartPos = particelSystem.RandomStartPos;
				myComponents.myParticleSystem.randSpread = particelSystem.randSpread;
				myComponents.myParticleSystem.RotateOverLifetime = particelSystem.RotateOverLifetime;
				strcpy_s(myComponents.myParticleSystem.ShaderName, particelSystem.UseShader->GetName().c_str());
				myComponents.myParticleSystem.Speed = particelSystem.Speed;
				myComponents.myParticleSystem.StartParticleSize = particelSystem.StartParticleSize;
				myComponents.myParticleSystem.StaticColor = particelSystem.StaticColor;
				myComponents.myParticleSystem.SystemEndColor = particelSystem.SystemEndColor;
				myComponents.myParticleSystem.SystemRotation = particelSystem.SystemRotation;
				myComponents.myParticleSystem.SystemStartColor = particelSystem.SystemStartColor;
				strcpy_s(myComponents.myParticleSystem.TextureName, particelSystem.Texture->GetName().c_str());
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
			//DropItem
			if (m_World->HasComponent<Frosty::ECS::CDropItem>(entity))
			{
				myComponents.MyComponents.at(11).HaveComponent = true;
				m_World->GetComponent<Frosty::ECS::CDropItem>(entity);
				//auto& dropItem = m_World->GetComponent<Frosty::ECS::CDropItem>(entity);
			}
			else
				myComponents.MyComponents.at(11).HaveComponent = false;
			//WitchCircle
			if (m_World->HasComponent<Frosty::ECS::CWitchCircle>(entity))
			{
				myComponents.MyComponents.at(12).HaveComponent = true;
				m_World->GetComponent<Frosty::ECS::CWitchCircle>(entity);
			}
			else
				myComponents.MyComponents.at(12).HaveComponent = false;
			//AnimController
			if (m_World->HasComponent<Frosty::ECS::CAnimController>(entity))
			{
				myComponents.MyComponents.at(13).HaveComponent = true;
				m_World->GetComponent<Frosty::ECS::CAnimController>(entity);
			}
			else
				myComponents.MyComponents.at(13).HaveComponent = false;
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
		myFile.write((const char*)& m_Header, sizeof(Level_Header));
		for (int i = 0; i < m_Entitys.myEntitys.size(); i++)
		{
			for (int j = 0; j < m_Header.NrOfComponents; j++)
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
						//11 = DropItem
					case 11:
						myFile.write((const char*)& m_Entitys.myEntitys.at(i).myDropItem, sizeof(Level_DropItem));
						break;
						//12 = WitchCircle
					case 12:
						myFile.write((const char*)& m_Entitys.myEntitys.at(i).myWitchCircle, sizeof(Level_WitchCircle));
						break;
					case 13:
						myFile.write((const char*)& m_Entitys.myEntitys.at(i).myAnimController, sizeof(Level_AnimController));
						break;
					default:
						break;
					}
				}
		}
		myFile.close();
		m_Entitys.myEntitys.clear();
		m_Header.NrOfEntitys = 0;


	}

	void LevelFileFormat::OpenFromFile(std::string fileName, glm::ivec2 roomId, Frosty::ECS::CTransform* playerTransform, int rotation, glm::vec3 move)
	{
		int physCounter = 0;
		Frosty::ECS::CTransform* planeTransform = nullptr;
		std::ifstream existingFile;
		existingFile.open("../../../assets/levels/" + fileName + ".lvl", std::ios::binary);
		Level_Header heder;
		Level_Entitys fileEntitys;
		std::vector<std::shared_ptr<Frosty::ECS::Entity>> m_Chests;
		std::vector<std::shared_ptr<Frosty::ECS::Entity>> m_WitchCirkel;
		FY_INFO("Opend file {0}", fileName);

		if (existingFile.good())
		{
			FY_INFO("it opend");
			existingFile.read((char*)& heder, sizeof(Level_Header));
			fileEntitys.myEntitys.resize(heder.NrOfEntitys);
			for (int i = 0; i < heder.NrOfEntitys; i++)
			{
				//to Fix my problen
				//if (i < 735)
				//{
				//	fileEntitys.myEntitys.at(i).MyComponents.resize(heder.NrOfComponents);
				//	for (int j = 0; j < heder.NrOfComponents; j++)
				//		existingFile.read((char*)& fileEntitys.myEntitys.at(i).MyComponents.at(j).HaveComponent, sizeof(bool));
				//	for (int j = 0; j < heder.NrOfComponents; j++)
				//	{
				//		if (fileEntitys.myEntitys.at(i).MyComponents.at(j).HaveComponent)
				//		{
				//
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

				fileEntitys.myEntitys.at(i).MyComponents.resize(m_Header.NrOfComponents);
				for (int j = 0; j < heder.NrOfComponents; j++)
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).MyComponents.at(j).HaveComponent, sizeof(bool));

				//create entity
				if (fileEntitys.myEntitys.at(i).MyComponents.at(0).HaveComponent)
				{
					//0 = Transform
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).myTransform, sizeof(Level_Transform));



					//temp cross
					//if (fileEntitys.myEntitys.at(i).myTransform.Scale.x == 270)
					//{
					//	fileEntitys.myEntitys.at(i).myTransform.Scale.x = 110.0f;
					//	fileEntitys.myEntitys.at(i).myTransform.Position.x = -81.0f;
					//}
					//if (fileEntitys.myEntitys.at(i).myTransform.Scale.z == 270)
					//{
					//	fileEntitys.myEntitys.at(i).myTransform.Scale.z = 110;
					//}

					glm::mat4 matrix(1.0f);
					matrix = glm::rotate(matrix, glm::radians((float)rotation), glm::vec3(0, 1, 0));
					matrix = glm::translate(matrix, fileEntitys.myEntitys.at(i).myTransform.Position);
					matrix = glm::rotate(matrix, fileEntitys.myEntitys.at(i).myTransform.Rotation.x, glm::vec3(1, 0, 0));
					matrix = glm::rotate(matrix, fileEntitys.myEntitys.at(i).myTransform.Rotation.y, glm::vec3(0, 1, 0));
					matrix = glm::rotate(matrix, fileEntitys.myEntitys.at(i).myTransform.Rotation.z, glm::vec3(0, 0, 1));
					//matrix = glm::scale(matrix, tranform.Scale);
					//temp ( becuse hitbox rotition dosent exist)
					glm::vec3 tempRotation = fileEntitys.myEntitys.at(i).myTransform.Rotation;
					if (rotation == 90 || rotation == 270)
					{
						if (fileEntitys.myEntitys.at(i).MyComponents.at(10).HaveComponent || !fileEntitys.myEntitys.at(i).MyComponents.at(1).HaveComponent)
						{
							float savedX = fileEntitys.myEntitys.at(i).myTransform.Scale.x;
							fileEntitys.myEntitys.at(i).myTransform.Scale.x = fileEntitys.myEntitys.at(i).myTransform.Scale.z *1.25f;
							fileEntitys.myEntitys.at(i).myTransform.Scale.z = savedX * 1.25f;
						}
						else
						{
							tempRotation.y += rotation;
						}
					}
					else if (rotation == 180)
					{
						tempRotation.y += rotation;
					}


					if (fileEntitys.myEntitys.at(i).MyComponents.at(10).HaveComponent)
					{
						fileEntitys.myEntitys.at(i).myTransform.Scale.x *= 1.25;
							fileEntitys.myEntitys.at(i).myTransform.Scale.z *= 1.25;
					}

					auto& entity = m_World->CreateEntity(glm::vec3(matrix[3].x, matrix[3].y, matrix[3].z), tempRotation, fileEntitys.myEntitys.at(i).myTransform.Scale, fileEntitys.myEntitys.at(i).myTransform.IsStatic);
					auto& newlyTreansform = m_World->GetComponent<Frosty::ECS::CTransform>(entity);
					if (newlyTreansform.Scale == glm::vec3(300.0f, 1.0f, 300.0f))
					{
						planeTransform = &newlyTreansform;
					}
					//1 = Mesh
					if (fileEntitys.myEntitys.at(i).MyComponents.at(1).HaveComponent)
					{
						existingFile.read((char*)& fileEntitys.myEntitys.at(i).myMesh, sizeof(Level_Mesh));
						std::string meshName = fileEntitys.myEntitys.at(i).myMesh.MeshName;
						//if (meshName.find("tiledGround") != std::string::npos)
						//{
						//	strcpy_s(fileEntitys.myEntitys.at(i).myMesh.MeshName, "pPlane1");
						//}
						//for in game
						//if(!fileEntitys.myEntitys.at(i).MyComponents.at(10).HaveComponent)


						if (!fileEntitys.myEntitys.at(i).MyComponents.at(10).HaveComponent)
						{

							m_World->AddComponent<Frosty::ECS::CMesh>(entity,
								Frosty::AssetManager::GetMesh(fileEntitys.myEntitys.at(i).myMesh.MeshName));
						}



						//std::string meshName = fileEntitys.myEntitys.at(i).myMesh.MeshName;
						//if (meshName.find("hexCircle") != std::string::npos)
						//{
						//	auto& particel = m_World->AddComponent<Frosty::ECS::CParticleSystem>(entity, "ParticlesHorizontal", "particleRing", 3, glm::vec3(0.1f, 0.5f, 0.58f), 0.0f);
						//	particel.SystemEndColor = glm::vec3(0.43f, 0.145f, 0.145f);
						//	particel.StartParticleSize = 3.0f;
						//	particel.EndParticleSize = 8.0f;
						//	particel.EmitRate = 1.0;
						//	particel.EmitCount = 1;
						//	particel.FadeInTreshold = 1.915;
						//	particel.FadeTreshold = 0.902;
						//	particel.ParticleSystemStartPos = glm::vec3(0, 0.03, 0);
						//}
						//else if (meshName.find("chest") != std::string::npos)
						//{
						//	m_World->AddComponent<Frosty::ECS::CLight>(entity, Frosty::ECS::CLight::LightType::Point, 1.0f, glm::vec3(1.0f, 0.99f, 0.95f), 5, glm::vec3(0.f, 1.0f, 0.f));
						//	////CParticleSystem(const std::string shaderName, const std::string texName, unsigned int maxParticles, const glm::vec3& color, float particleSpeed)
						//	auto& particel = m_World->AddComponent<Frosty::ECS::CParticleSystem>(entity, "ParticlesHorizontal", "particle", 3, glm::vec3(1.0f, 0.96f, 0.0f), 0);
						//	particel.EndParticleSize = 10;
						//	particel.FadeInTreshold = 0.94f;
						//	particel.FadeTreshold = 1.32f;
						//	particel.EmitRate = 1;
						//}
					}
					//2 = Material
					if (fileEntitys.myEntitys.at(i).MyComponents.at(2).HaveComponent)
					{
						existingFile.read((char*)& fileEntitys.myEntitys.at(i).myMaterial, sizeof(Level_Material));

						//if (fileEntitys.myEntitys.at(i).MyComponents.at(6).HaveComponent)
						//{
						//	std::string meshName = fileEntitys.myEntitys.at(i).myMesh.MeshName;
						//	if (meshName.find("Wolf") != std::string::npos)
						//	{
						//		m_World->AddComponent<Frosty::ECS::CAnimController>(entity).currAnim = Frosty::AssetManager::GetAnimation("Wolf_Idle");
						//	}
						//	else if (meshName.find("Cultist") != std::string::npos)
						//	{
						//		m_World->AddComponent<Frosty::ECS::CAnimController>(entity).currAnim = Frosty::AssetManager::GetAnimation("Cultist_Idle");
						//	}
						//	strcpy_s(fileEntitys.myEntitys.at(i).myMaterial.UseShaderName, "Animation");
						//}
						bool hasTransparency = false;
						std::string meshName = fileEntitys.myEntitys.at(i).myMaterial.DiffuseTextureName;
						if (m_World->HasComponent<Frosty::ECS::CMesh>(entity) && meshName.find("tree") != std::string::npos)
							hasTransparency = true;
						if (m_World->HasComponent<Frosty::ECS::CMesh>(entity) && meshName.find("Tree") != std::string::npos)
							hasTransparency = true;

						auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(entity,
							Frosty::AssetManager::GetShader(fileEntitys.myEntitys.at(i).myMaterial.UseShaderName), hasTransparency);
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
						material.HasTransparency = fileEntitys.myEntitys.at(i).myMaterial.HasTransparency;
					
					}
					//3 = Follow
					if (fileEntitys.myEntitys.at(i).MyComponents.at(3).HaveComponent)
					{
						existingFile.read((char*)& fileEntitys.myEntitys.at(i).myFollow, sizeof(Level_Follow));
						//auto& follow = m_World->AddComponent<Frosty::ECS::CFollow>(entity, playerTransform);
						//For edeting old level
						//auto& follow = m_World->AddComponent<Frosty::ECS::CFollow>(entity);// , playerTransform);
						//follow.StopDistance = fileEntitys.myEntitys.at(i).myFollow.StopDistance;
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

						physCounter++;
						auto& physics = m_World->AddComponent<Frosty::ECS::CPhysics>(entity,
							Frosty::AssetManager::GetBoundingBox(fileEntitys.myEntitys.at(i).MyComponents.at(1).HaveComponent ?
								fileEntitys.myEntitys.at(i).myMesh.MeshName : "pCube1"), newlyTreansform.Scale);
						physics.Direction = fileEntitys.myEntitys.at(i).myPhysics.Direction;
						physics.MaxSpeed = fileEntitys.myEntitys.at(i).myPhysics.MaxSpeed;
						physics.Speed = fileEntitys.myEntitys.at(i).myPhysics.Speed;
						physics.SpeedMultiplier = fileEntitys.myEntitys.at(i).myPhysics.SpeedMultiplier;


						//physics.Velocity = fileEntitys.myEntitys.at(i).myPhysics.Velocity;
					}
					//6 = Enemy
					if (fileEntitys.myEntitys.at(i).MyComponents.at(6).HaveComponent)
					{
						//temp wepond
						existingFile.read((char*)& fileEntitys.myEntitys.at(i).myEnemy, sizeof(Level_Enemy));
						std::string meshName = fileEntitys.myEntitys.at(i).myMesh.MeshName;
						auto& enemyWeaponA = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
						auto& weaponHandler = Frosty::AssetManager::GetWeaponHandler("Weapons");
						Frosty::Weapon loadedWeapon;
						int lowLevel = 1;
						int highLevel = 1;
						if (m_VisitedRooms.size() < 2)
						{
							lowLevel = 1;
							highLevel = 1;
						}
						else if (m_VisitedRooms.size() < 4)
						{
							lowLevel = 1;
							highLevel = 2;
						}
						else if (m_VisitedRooms.size() < 5)
						{
							lowLevel = 2;
							highLevel = 2;
						}
						else if (m_VisitedRooms.size() < 7)
						{
							lowLevel = 2;
							highLevel = 3;
						}
						else if (m_VisitedRooms.size() > 6)
						{
							lowLevel = 3;
							highLevel = 3;
						}
						else
						{
							lowLevel = 1;
							highLevel = 3;
						}
						if (meshName.find("Cultist") != std::string::npos)
						{
							////world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("pCube1"));
							////world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("FlatColor"));
							//auto& enemyWeaponCompA = m_World->AddComponent<Frosty::ECS::CWeapon>(enemyWeaponA, Frosty::ECS::CWeapon::WeaponType::Bow, 1, 1.0f);
							//enemyWeaponCompA.LVL1AttackCooldown = 3.0f;
							////enemyWeaponCompA.MaxAttackRange = 5.0f;
							////enemyWeaponCompA.MinAttackRange = 0.0f;
							//enemyWeaponCompA.MaxAttackRange = 22.0f;
							//enemyWeaponCompA.MinAttackRange = 18.0f;
							////enemyWeaponCompA.AttackHitboxScale = glm::vec3(10.0f, 6.0f, 4.0f);				// Sword
							////enemyWeaponCompA.AttackHitboxScale = glm::vec3(4.0f, 6.0f, 4.0f);				// Bite
							//enemyWeaponCompA.AttackHitboxScale = glm::vec3(0.3f);							// Arrow
							//auto& enemy = m_World->AddComponent<Frosty::ECS::CEnemy>(entity, playerTransform, &enemyWeaponCompA, fileEntitys.myEntitys.at(i).myEnemy.RunOnHealth);
							//auto& transform = m_World->GetComponent< Frosty::ECS::CTransform>(entity);
							//enemy.SpawnPosition = transform.Position;
							int isMelle = rand() % 2;
							if (isMelle == 0)
							{
								loadedWeapon = weaponHandler->GetWeaponByTypeAndLevel(Frosty::Weapon::WeaponType::Bow, lowLevel, highLevel);

								//m_World->AddComponent<Frosty::ECS::CMesh>(enemyWeaponA, Frosty::AssetManager::GetMesh("Bow"));
								//auto& weaponMat = m_World->AddComponent<Frosty::ECS::CMaterial>(enemyWeaponA, Frosty::AssetManager::GetShader("Texture2D"));
								//weaponMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("bow_lvl1_diffuse");
								//weaponMat.NormalTexture = Frosty::AssetManager::GetTexture2D("bow_normal");
							}
							if (isMelle == 1)
							{
								loadedWeapon = weaponHandler->GetWeaponByTypeAndLevel(Frosty::Weapon::WeaponType::Sword, lowLevel, highLevel);
							}

							//if (weaponComp.Type == Frosty::ECS::CWeapon::WeaponType::Bow)
							//{

							//	weaponMesh = &world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("Bow"));
							//	auto& weaponMat = world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("Texture2D"));
							//	weaponMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("bow_lvl1_diffuse");
							//	weaponMat.NormalTexture = Frosty::AssetManager::GetTexture2D("bow_normal");
							//}
							//else
							//{

							//	weaponMesh = &world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("sword"));
							//	auto& weaponMat = world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("Texture2D"));
							//	weaponMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("sword_lvl1_diffuse");
							//	weaponMat.NormalTexture = Frosty::AssetManager::GetTexture2D("sword_normal");
							//}
						}
						else if (meshName.find("Wolf") != std::string::npos)
						{
							////world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("pCube1"));
							////world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("FlatColor"));
							//auto& enemyWeaponCompA = m_World->AddComponent<Frosty::ECS::CWeapon>(enemyWeaponA, Frosty::ECS::CWeapon::WeaponType::Bite, 1, 1.0f);
							//enemyWeaponCompA.LVL1AttackCooldown = 3.0f;
							//enemyWeaponCompA.MaxAttackRange = 5.0f;
							//enemyWeaponCompA.MinAttackRange = 0.0f;
							////enemyWeaponCompA.MaxAttackRange = 22.0f;
							////enemyWeaponCompA.MinAttackRange = 18.0f;
							////enemyWeaponCompA.AttackHitboxScale = glm::vec3(10.0f, 6.0f, 4.0f);				// Sword
							//enemyWeaponCompA.AttackHitboxScale = glm::vec3(4.0f, 6.0f, 4.0f);				// Bite
							////enemyWeaponCompA.AttackHitboxScale = glm::vec3(0.3f);

							//auto& enemy = m_World->AddComponent<Frosty::ECS::CEnemy>(entity, playerTransform, &enemyWeaponCompA, fileEntitys.myEntitys.at(i).myEnemy.RunOnHealth);
							//auto& transform = m_World->GetComponent< Frosty::ECS::CTransform>(entity);
							//enemy.SpawnPosition = transform.Position;


							loadedWeapon = weaponHandler->GetWeaponByTypeAndLevel(Frosty::Weapon::WeaponType::Bite, lowLevel, highLevel);
							auto& physics = m_World->GetComponent<Frosty::ECS::CPhysics>(entity);
							physics.Speed = 10.0f;
						}
						auto& enemyWeaponCompA = m_World->AddComponent<Frosty::ECS::CWeapon>(enemyWeaponA, loadedWeapon);
						auto& enemy = m_World->AddComponent<Frosty::ECS::CEnemy>(entity, playerTransform, &enemyWeaponCompA, fileEntitys.myEntitys.at(i).myEnemy.RunOnHealth);
						enemy.WeaponEntityID = enemyWeaponCompA.EntityPtr->Id;
						auto& transform = m_World->GetComponent< Frosty::ECS::CTransform>(entity);
						enemy.SpawnPosition = transform.Position;
					}
					//7 = Health
					if (fileEntitys.myEntitys.at(i).MyComponents.at(7).HaveComponent)
					{
						existingFile.read((char*)& fileEntitys.myEntitys.at(i).myHealth, sizeof(Level_Health));
						auto& health = m_World->AddComponent<Frosty::ECS::CHealth>(entity);
						health.CurrentHealth = fileEntitys.myEntitys.at(i).myHealth.CurrentHealth;
						health.MaxHealth = fileEntitys.myEntitys.at(i).myHealth.MaxHealth;
						health.MaxPossibleHealth = fileEntitys.myEntitys.at(i).myHealth.MaxPossibleHealth;
						//if (fileEntitys.myEntitys.at(i).MyComponents.at(6).HaveComponent)
						//{
						//	if (m_VisitedRooms.size() < 2)
						//	{
						//		health.CurrentHealth -= health.CurrentHealth * 0.4;
						//		health.MaxHealth -= health.MaxHealth * 0.4;
						//	}
						//	if (m_VisitedRooms.size() < 4)
						//	{
						//		health.CurrentHealth -= health.CurrentHealth * 0.2;
						//		health.MaxHealth -= health.MaxHealth * 0.2;
						//	}
						//}

						if (fileEntitys.myEntitys.at(i).MyComponents.at(12).HaveComponent)
						{
							health.CurrentHealth /= 2;
							health.MaxHealth /= 2;
						}

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
						auto& particleSystem = m_World->AddComponent<Frosty::ECS::CParticleSystem>(entity,
							fileEntitys.myEntitys.at(i).myParticleSystem.ShaderName,
							fileEntitys.myEntitys.at(i).myParticleSystem.TextureName,
							fileEntitys.myEntitys.at(i).myParticleSystem.MaxParticles,
							fileEntitys.myEntitys.at(i).myParticleSystem.SystemStartColor,
							fileEntitys.myEntitys.at(i).myParticleSystem.Speed);
						particleSystem.StartParticleSize = fileEntitys.myEntitys.at(i).myParticleSystem.StartParticleSize;
						particleSystem.EndParticleSize = fileEntitys.myEntitys.at(i).myParticleSystem.EndParticleSize;
						particleSystem.SystemEndColor = fileEntitys.myEntitys.at(i).myParticleSystem.SystemEndColor;
						particleSystem.AlwaysFaceCamera = fileEntitys.myEntitys.at(i).myParticleSystem.AlwaysFaceCamera;
						particleSystem.EmitCount = fileEntitys.myEntitys.at(i).myParticleSystem.EmitCount;
						particleSystem.EmitRate = fileEntitys.myEntitys.at(i).myParticleSystem.EmitRate;
						particleSystem.FadeInTreshold = fileEntitys.myEntitys.at(i).myParticleSystem.FadeInTreshold;
						particleSystem.FadeTreshold = fileEntitys.myEntitys.at(i).myParticleSystem.FadeTreshold;
						particleSystem.MaxLifetime = fileEntitys.myEntitys.at(i).myParticleSystem.MaxLifetime;
						particleSystem.MinLifetime = fileEntitys.myEntitys.at(i).myParticleSystem.MinLifetime;
						particleSystem.ParticleCount = fileEntitys.myEntitys.at(i).myParticleSystem.ParticleCount;
						particleSystem.ParticleSize = fileEntitys.myEntitys.at(i).myParticleSystem.ParticleSize;
						particleSystem.ParticleSystemDirection = fileEntitys.myEntitys.at(i).myParticleSystem.ParticleSystemDirection;
						particleSystem.ParticleSystemStartPos = fileEntitys.myEntitys.at(i).myParticleSystem.ParticleSystemStartPos;
						particleSystem.randMainDir = fileEntitys.myEntitys.at(i).myParticleSystem.randMainDir;
						particleSystem.RandomDirection = fileEntitys.myEntitys.at(i).myParticleSystem.RandomDirection;
						particleSystem.RandomLifetimes = fileEntitys.myEntitys.at(i).myParticleSystem.RandomLifetimes;
						particleSystem.RandomStartPos = fileEntitys.myEntitys.at(i).myParticleSystem.RandomStartPos;
						particleSystem.randSpread = fileEntitys.myEntitys.at(i).myParticleSystem.randSpread;
						particleSystem.RotateOverLifetime = fileEntitys.myEntitys.at(i).myParticleSystem.RotateOverLifetime;
						particleSystem.StaticColor = fileEntitys.myEntitys.at(i).myParticleSystem.StaticColor;
						particleSystem.SystemRotation = fileEntitys.myEntitys.at(i).myParticleSystem.SystemRotation;
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
					//11 = DropItem
					if (fileEntitys.myEntitys.at(i).MyComponents.at(11).HaveComponent)
					{
						existingFile.read((char*)& fileEntitys.myEntitys.at(i).myDropItem, sizeof(Level_DropItem));
						m_World->AddComponent<Frosty::ECS::CDropItem>(entity);
						std::string meshName = fileEntitys.myEntitys.at(i).myMesh.MeshName;
						if (meshName.find("chest") != std::string::npos)
							m_Chests.push_back(entity);
					}
					//12 = WitchCircle
					if (fileEntitys.myEntitys.at(i).MyComponents.at(12).HaveComponent)
					{
						existingFile.read((char*)& fileEntitys.myEntitys.at(i).myWitchCircle, sizeof(Level_WitchCircle));
						m_World->AddComponent<Frosty::ECS::CWitchCircle>(entity);
						m_WitchCirkel.push_back(entity);
					}
					//13 = AnimController
					if (fileEntitys.myEntitys.at(i).MyComponents.at(13).HaveComponent)
					{
						existingFile.read((char*)& fileEntitys.myEntitys.at(i).myWitchCircle, sizeof(Level_AnimController));
						std::string meshName = fileEntitys.myEntitys.at(i).myMesh.MeshName;
						if (meshName.find("Wolf") != std::string::npos)
						{
							m_World->AddComponent<Frosty::ECS::CAnimController>(entity).currAnim = Frosty::AssetManager::GetAnimation("Wolf_Idle");
						}
						else if (meshName.find("Cultist") != std::string::npos)
						{
							m_World->AddComponent<Frosty::ECS::CAnimController>(entity).currAnim = Frosty::AssetManager::GetAnimation("Cultist_Idle");
						}
						else
						{
							m_World->AddComponent<Frosty::ECS::CAnimController>(entity);
						}
					}
				}
			}

			//to remove "enemys" or chest m.m to control the number and randomize pos
			int enteredRoomId = -1;
			for (int i = 0; i < m_VisitedRooms.size(); i++)
			{
				if (m_VisitedRooms.at(i).myRoomId == roomId)
				{
					enteredRoomId = i;
				}
			}
			if (enteredRoomId != -1)
			{
				//removes the number of chests
				for (int i = 0; i < m_VisitedRooms.at(enteredRoomId).removeChest.size(); i++)
				{
					if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Chests.at(m_VisitedRooms.at(enteredRoomId).removeChest.at(i))))
					{
						m_World->AddComponent<Frosty::ECS::CDestroy>(m_Chests.at(m_VisitedRooms.at(enteredRoomId).removeChest.at(i)));
					}
					std::shared_ptr<Frosty::ECS::Entity> temp = m_Chests.at(m_VisitedRooms.at(enteredRoomId).removeChest.at(i));
					m_Chests.at(m_VisitedRooms.at(enteredRoomId).removeChest.at(i)) = m_Chests.at(m_Chests.size() - 1 - i);
					m_Chests.back() = temp;
				}
				//removes the number of witchCirkels
				for (int i = 0; i < m_VisitedRooms.at(enteredRoomId).removeWitchCirkel.size(); i++)
				{
					if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_WitchCirkel.at(m_VisitedRooms.at(enteredRoomId).removeWitchCirkel.at(i))))
					{
						m_World->AddComponent<Frosty::ECS::CDestroy>(m_WitchCirkel.at(m_VisitedRooms.at(enteredRoomId).removeWitchCirkel.at(i)));
					}
					std::shared_ptr<Frosty::ECS::Entity> temp = m_Chests.at(m_VisitedRooms.at(enteredRoomId).removeWitchCirkel.at(i));
					m_WitchCirkel.at(m_VisitedRooms.at(enteredRoomId).removeWitchCirkel.at(i)) = m_WitchCirkel.at(m_WitchCirkel.size() - 1 - i);
					m_WitchCirkel.back() = temp;
				}
			}
			else
			{
				int nrToHave = rand() % 3;
				if(roomId == glm::ivec2(10, 15))
					nrToHave = 2;
				Level_rememberedEntitys rEntitys;
				rEntitys.myRoomId = roomId;
				//removes the number of chests
				if (m_Chests.size() >= nrToHave)
				{
					for (int i = 0; i < m_Chests.size() - nrToHave; i++)
					{
						int rnd = rand() % (m_Chests.size() - i);
						if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Chests.at(rnd)))
						{
							rEntitys.removeChest.push_back(rnd);
							m_World->AddComponent<Frosty::ECS::CDestroy>(m_Chests.at(rnd));
						}
						std::shared_ptr<Frosty::ECS::Entity> temp = m_Chests.at(rnd);
						m_Chests.at(rnd) = m_Chests.at(m_Chests.size() - 1 - i);
						m_Chests.back() = temp;
					}
				}
				//removes the number of witchCirkels
				nrToHave = rand() % 3;
				if (nrToHave > 0)
					nrToHave = 1;
				if (m_WitchCirkel.size() >= nrToHave && roomId != glm::ivec2(10, 15))
				{
					for (int i = 0; i < m_WitchCirkel.size() - nrToHave; i++)
					{
						int rnd = rand() % (m_WitchCirkel.size() - i);
						if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_WitchCirkel.at(rnd)))
						{
							rEntitys.removeWitchCirkel.push_back(rnd);
							m_World->AddComponent<Frosty::ECS::CDestroy>(m_WitchCirkel.at(rnd));
						}
						std::shared_ptr<Frosty::ECS::Entity> temp = m_WitchCirkel.at(rnd);
						m_WitchCirkel.at(rnd) = m_WitchCirkel.at(m_WitchCirkel.size() - 1 - i);
						m_WitchCirkel.back() = temp;
					}
				}
				m_VisitedRooms.push_back(rEntitys);
			}
		}
		else
		{
			FY_INFO("it diden't exist");
		}
		existingFile.close();

		if (planeTransform != nullptr)
		{
			Frosty::EventBus::GetEventBus()->Publish<Frosty::InitiateGridEvent>(Frosty::InitiateGridEvent(planeTransform));
		}
		else
		{
			FY_INFO("Did not fined a plain for {0}", fileName);
		}
	}



	void LevelFileFormat::LoadBoolMap(std::string fileName)
		 
	{
		std::ifstream existingFile;
		existingFile.open("../../../assets/levels/" + fileName + ".lvl", std::ios::binary);
		Level_Header testHeder;
		Level_Entitys fileEntitys;


		std::unordered_map<std::string, Frosty::VABatch> TestMap;

		if (existingFile.good())
		{

			existingFile.read((char*)& testHeder, sizeof(Level_Header));
			fileEntitys.myEntitys.resize(testHeder.NrOfEntitys);
			for (int i = 0; i < testHeder.NrOfEntitys; i++)
			{

				bool MeshAdded = false;
				std::vector<std::string> AddedMeshes;
				std::unordered_map<std::string, Frosty::VABatch> TestMap;

				fileEntitys.myEntitys.at(i).MyComponents.resize(m_Header.NrOfComponents);
				for (int j = 0; j < testHeder.NrOfComponents; j++)
				{
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).MyComponents.at(j).HaveComponent, sizeof(bool));
				}





				if (fileEntitys.myEntitys.at(i).MyComponents.at(0).HaveComponent)
				{
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).myTransform, sizeof(Level_Transform));



				}



				if (fileEntitys.myEntitys.at(i).MyComponents.at(1).HaveComponent)
				{
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).myMesh, sizeof(Level_Mesh));

					if (!TestMap.count(fileEntitys.myEntitys.at(i).myMesh.MeshName))
					{
						Frosty::VABatch Temp;
						//Change to ptr
						Temp.VertexArrayObj = Frosty::AssetManager::GetMesh(fileEntitys.myEntitys.at(i).myMesh.MeshName);


						glm::mat4 TempMat(1.0f);
						glm::vec3 Offset(150.0f, 0.0f, 150.0f);
						TempMat = glm::translate(TempMat, fileEntitys.myEntitys.at(i).myTransform.Position + Offset);
						TempMat = glm::rotate(TempMat, glm::radians(fileEntitys.myEntitys.at(i).myTransform.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
						TempMat = glm::scale(TempMat, fileEntitys.myEntitys.at(i).myTransform.Scale);
						Temp.Transforms.emplace_back(TempMat);
						TestMap[fileEntitys.myEntitys.at(i).myMesh.MeshName] = Temp;
					}
					else
					{

						glm::mat4 TempMat(1.0f);

						glm::vec3 Offset(150.0f, 0.0f, 150.0f);
						TempMat = glm::translate(TempMat, fileEntitys.myEntitys.at(i).myTransform.Position + Offset);
						TempMat = glm::rotate(TempMat, glm::radians(fileEntitys.myEntitys.at(i).myTransform.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
						TempMat = glm::scale(TempMat, fileEntitys.myEntitys.at(i).myTransform.Scale);

						TestMap[fileEntitys.myEntitys.at(i).myMesh.MeshName].Transforms.emplace_back(TempMat);
					}


				}


				if (fileEntitys.myEntitys.at(i).MyComponents.at(2).HaveComponent)
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).myMaterial, sizeof(Level_Material));
				if (fileEntitys.myEntitys.at(i).MyComponents.at(3).HaveComponent)
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).myFollow, sizeof(Level_Follow));
				if (fileEntitys.myEntitys.at(i).MyComponents.at(4).HaveComponent)
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).myLight, sizeof(Level_Light));
				if (fileEntitys.myEntitys.at(i).MyComponents.at(5).HaveComponent)
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).myPhysics, sizeof(Level_Physics));
				if (fileEntitys.myEntitys.at(i).MyComponents.at(6).HaveComponent)
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).myEnemy, sizeof(Level_Enemy));
				if (fileEntitys.myEntitys.at(i).MyComponents.at(7).HaveComponent)
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).myHealth, sizeof(Level_Health));
				if (fileEntitys.myEntitys.at(i).MyComponents.at(8).HaveComponent)
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).myHealthBar, sizeof(Level_HealthBar));
				if (fileEntitys.myEntitys.at(i).MyComponents.at(9).HaveComponent)
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).myParticleSystem, sizeof(Level_ParticleSystem));
				if (fileEntitys.myEntitys.at(i).MyComponents.at(10).HaveComponent)
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).myLevelExit, sizeof(Level_LevelExit));

				//11 = DropItem
				if (fileEntitys.myEntitys.at(i).MyComponents.at(11).HaveComponent)
				{
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).myDropItem, sizeof(Level_DropItem));

				}
				//12 = WitchCircle
				if (fileEntitys.myEntitys.at(i).MyComponents.at(12).HaveComponent)
				{
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).myWitchCircle, sizeof(Level_WitchCircle));

				}
				//13 = AnimController
				if (fileEntitys.myEntitys.at(i).MyComponents.at(13).HaveComponent)
				{
					existingFile.read((char*)& fileEntitys.myEntitys.at(i).myAnimController, sizeof(Level_AnimController));

				}
			}
			existingFile.close();

		}




		std::unordered_map<std::string, Frosty::VABatch>::iterator it = TestMap.begin();
		while (it != TestMap.end())
		{
			Frosty::BoolMapGenerator::AddBatch(it->second);
			it++;
		}

		std::shared_ptr<BoolMap> ABoolMap = Frosty::BoolMapGenerator::RenderBoolMap();
		//for (int i = 0; i < 300; i++)
		//{
		//	for (int j = 0; j < 300; j++)
		//	{
		//		bool k = ABoolMap->CheckCollition(glm::vec3((float)i, 10.0f, (float)j));
		//		if (k)
		//		{
		//			int yay = 0;
		//		}
		//	}
		//}
		bool k = ABoolMap->CheckCollition(glm::vec3(1.0f, 0.0f, 1.0f));
		ABoolMap->SaveMap("", "BoolMap");
		ABoolMap->LoadMap("BoolMap.bmap");
	}
}

