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
		}
		else
			myComponents.MyComponents.at(1).HaveComponent = false;
		m_Entitys.myEntitys.push_back(myComponents);
	}
}



