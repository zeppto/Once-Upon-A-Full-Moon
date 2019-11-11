#include <fypch.hpp>
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
			auto& tranform = m_World->GetComponent<Frosty::ECS::CTransform>(entity);
			myComponents.MyComponents.at(0).HaveComponent = true;
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
			myComponents.myMesh.MeshName =
		}
		else
			myComponents.MyComponents.at(1).HaveComponent = false;
		//Material
		if (m_World->HasComponent<Frosty::ECS::CMaterial>(entity))
		{
			myComponents.MyComponents.at(2).HaveComponent = true;
		}
		else
			myComponents.MyComponents.at(2).HaveComponent = false;
		//Follow
		if (m_World->HasComponent<Frosty::ECS::CFollow>(entity))
		{
			myComponents.MyComponents.at(3).HaveComponent = true;
		}
		else
			myComponents.MyComponents.at(3).HaveComponent = false;
		//Light
		if (m_World->HasComponent<Frosty::ECS::CLight>(entity))
		{
			myComponents.MyComponents.at(4).HaveComponent = true;
		}
		else
			myComponents.MyComponents.at(4).HaveComponent = false;
		//Physics
		if (m_World->HasComponent<Frosty::ECS::CPhysics>(entity))
		{
			myComponents.MyComponents.at(5).HaveComponent = true;
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
		}
		else
			myComponents.MyComponents.at(7).HaveComponent = false;
		//HealthBar
		if (m_World->HasComponent<Frosty::ECS::CHealthBar>(entity))
		{
			myComponents.MyComponents.at(8).HaveComponent = true;
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
		}
		else
			myComponents.MyComponents.at(10).HaveComponent = false;

		m_Entitys.myEntitys.push_back(myComponents);
	}
}



