#include <mcspch.hpp>
#include "Frosty.h"
#include "LevelFileFormat.hpp"


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
			myComponents.myTransform.ModelMatrix = tranform.ModelMatrix;
		}
		else
			myComponents.MyComponents.at(0).HaveComponent = false;
		//Mesh
		if (m_World->HasComponent<Frosty::ECS::CMesh>(entity))
		{
			myComponents.MyComponents.at(1).HaveComponent = true;
			auto& mesh = m_World->GetComponent<Frosty::ECS::CMesh>(entity);
			myComponents.myMesh.MeshName = mesh.Mesh->GetName();
		}
		else
			myComponents.MyComponents.at(1).HaveComponent = false;
		//Material
		if (m_World->HasComponent<Frosty::ECS::CMaterial>(entity))
		{
			myComponents.MyComponents.at(2).HaveComponent = true;
			auto& material = m_World->GetComponent<Frosty::ECS::CMaterial>(entity);
			myComponents.myMaterial.UseShaderName = material.UseShader->GetName();
			myComponents.myMaterial.Albedo = material.Albedo;
			if(material.DiffuseTexture != nullptr)
				myComponents.myMaterial.DiffuseTextureName = material.DiffuseTexture->GetName();
			if (material.SpecularTexture != nullptr)
				myComponents.myMaterial.SpecularTextureName = material.SpecularTexture->GetName();
			if (material.NormalTexture != nullptr)
				myComponents.myMaterial.NormalTextureName = material.NormalTexture->GetName();
			if (material.BlendMapTexture != nullptr)
				myComponents.myMaterial.BlendMapTextureName = material.BlendMapTexture->GetName();
			if (material.BlendTexture1 != nullptr)
				myComponents.myMaterial.BlendTexture1Name = material.BlendTexture1->GetName();
			if (material.BlendTexture2 != nullptr)
				myComponents.myMaterial.BlendTexture2Name = material.BlendTexture2->GetName();
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
			for (int i = 0; i < 3; i++)
			{
				myComponents.myPhysics.BoundingBox.halfSize[i] = physics.BoundingBox->halfSize[i];
				myComponents.myPhysics.BoundingBox.pos[i] = physics.BoundingBox->pos[i];
			}
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
			myComponents.myHealthBar.HpTransform = healthBar.hpTransform;
			myComponents.myHealthBar.MeshName = healthBar.Mesh->GetName();
			myComponents.myHealthBar.TextureName = healthBar.Texture->GetName();
			myComponents.myHealthBar.UseShaderName = healthBar.UseShader->GetName();
		}
		else
			myComponents.MyComponents.at(8).HaveComponent = false;
		//ParticleSystem
		if (m_World->HasComponent<Frosty::ECS::CParticleSystem>(entity))
		{
			myComponents.MyComponents.at(9).HaveComponent = true;
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

void LevelFileFormat::SaveToFile()
{
	m_Entitys;
	m_Header;

	std::ofstream myFile;
	myFile.open("../../../assets/levels/lovol.lvl", std::ios::binary);
	myFile << "Writing this to a file.\n";
	myFile.close();


}



