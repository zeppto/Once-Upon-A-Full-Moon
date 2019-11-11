#include <mcspch.hpp>
#include "GridMap.hpp"

// Temporary
#include "Frosty/API/AssetManager/AssetManager.hpp"

namespace MCS
{

	const int GridMap::CELL_SIZE;

	void GridMap::Init(const Frosty::ECS::CTransform& transform)
	{
		// Temporary
		auto& world = Frosty::Application::Get().GetWorld();
		//auto& physicsCompData = world-><Frosty::ECS::CPhysics>()->GetAll();
		//auto* physicsCompManager = world->GetComponentManager<Frosty::ECS::CPhysics>();
		auto& physicsCompData = world->GetComponentManager<Frosty::ECS::CPhysics>()->GetAll();
		size_t totalData = world->GetComponentManager<Frosty::ECS::CPhysics>()->GetTotal();
		
		// Check which entities should be included as obstacles 
		std::vector<Frosty::ECS::CTransform*> transforms;
		for (size_t i = 1; i < totalData; i++)
		{
			auto& tempTransform = world->GetComponent<Frosty::ECS::CTransform>(physicsCompData[i].EntityPtr);
			if (tempTransform.IsStatic)
			{
				// This means that the entity is static and has a CPhysics component
				transforms.emplace_back(&tempTransform);
			}
		}


		// 
		m_Position = transform.Position;
		m_Scale = transform.Scale;

		// Calculate and reserve space for amount of cells
		int totalCells = (int)(m_Scale.x * m_Scale.z / glm::pow((float)CELL_SIZE, 2));
		m_Cells.reserve(totalCells);

		// Initiate cells
		for (int z = 1; z <= m_Scale.z / CELL_SIZE; z++)
		{
			for (int x = 1; x <= m_Scale.x / CELL_SIZE; x++)
			{
				glm::vec2 localCellPos = glm::vec2(x * (CELL_SIZE / 2.0f) + (x - 1) * CELL_SIZE / 2.0f, z * (CELL_SIZE / 2.0f) + (z - 1) * CELL_SIZE / 2.0f);
				m_Cells.emplace_back(Cell(localCellPos, GetWorldPosition(localCellPos)));

				//auto& cell = world->CreateEntity({ m_Cells.back().WorldPosition.x, 0.01f, m_Cells.back().WorldPosition.z }, { 0.0f, 0.0f, 0.0f }, { CELL_SIZE, 1.0f, CELL_SIZE }, true);
				//world->AddComponent<Frosty::ECS::CMesh>(cell, Frosty::AssetManager::GetMesh("pPlane1"));
				//auto& cellMat = world->AddComponent<Frosty::ECS::CMaterial>(cell, Frosty::AssetManager::GetShader("FlatColor"));
				//cellMat.Albedo = glm::vec4(m_Cells.back().LocalPosition.x / m_Scale.x, 0.0f, m_Cells.back().LocalPosition.y / m_Scale.z, 1.0f);
			}
		}

		// Set cell collision
		for (size_t i = 0; i < transforms.size(); i++)
		{
			auto& tempPhysics = world->GetComponent<Frosty::ECS::CPhysics>(transforms[i]->EntityPtr);
			glm::vec4 collisionBox = glm::vec4(
				transforms[i]->Position.x - transforms[i]->Scale.x * tempPhysics.BoundingBox->halfSize[0],
				transforms[i]->Position.z + transforms[i]->Scale.z * tempPhysics.BoundingBox->halfSize[2],
				transforms[i]->Position.x + transforms[i]->Scale.x * tempPhysics.BoundingBox->halfSize[0],
				transforms[i]->Position.z - transforms[i]->Scale.z * tempPhysics.BoundingBox->halfSize[2]
			);

			glm::vec2 localPos = GetLocalPosition({ collisionBox.x, 0.0f, collisionBox.y });
			glm::vec3 worldPos = GetWorldPosition(localPos);

			size_t cellIndex = GetCellIndex(localPos);

			size_t cellsInZ = (size_t)(glm::floor((collisionBox.y - collisionBox.w) / CELL_SIZE));
			size_t cellsInX = (size_t)(glm::floor((collisionBox.z - collisionBox.x) / CELL_SIZE));

			for (size_t z = 0; z <= cellsInZ; z++)
			{
				size_t cellIndexStart = cellIndex + (size_t)(m_Scale.x / CELL_SIZE) * z;
				for (size_t x = 0; x <= cellsInX; x++)
				{
					m_Cells[cellIndexStart + x].weight = -1;
				}
			}
			
		}

		//for (size_t i = 0; i < m_Cells.size(); i++)
		//{
		//	if (m_Cells[i].weight >= 0)
		//	{
		//		auto& cell = world->CreateEntity({ m_Cells[i].WorldPosition.x, 0.01f, m_Cells[i].WorldPosition.z }, { 0.0f, 0.0f, 0.0f }, { CELL_SIZE, 1.0f, CELL_SIZE }, true);
		//		world->AddComponent<Frosty::ECS::CMesh>(cell, Frosty::AssetManager::GetMesh("pPlane1"));
		//		auto& cellMat = world->AddComponent<Frosty::ECS::CMaterial>(cell, Frosty::AssetManager::GetShader("FlatColor"));
		//		cellMat.Albedo = glm::vec4(m_Cells[i].LocalPosition.x / m_Scale.x, 0.0f, m_Cells[i].LocalPosition.y / m_Scale.z, 1.0f);
		//	}
		//}

		//system("pause");
	}

	glm::vec3 GridMap::GetWorldPosition(const glm::vec2& localPos) const
	{
		glm::vec2 cornerPos = glm::vec2(m_Position.x - m_Scale.x * 0.5f, m_Position.z + m_Scale.z * 0.5f);
		return glm::vec3(localPos.x + cornerPos.x, m_Position.y,  cornerPos.y - localPos.y);
	}

	glm::vec2 GridMap::GetLocalPosition(const glm::vec3& worldPos) const
	{
		glm::vec2 cornerPos = glm::vec2(m_Position.x - m_Scale.x * 0.5f, m_Position.z + m_Scale.z * 0.5f);
		return glm::vec2(glm::abs(worldPos.x - cornerPos.x), glm::abs(worldPos.z - cornerPos.y));
	}

	size_t GridMap::GetCellIndex(const glm::vec2& localPos) const
	{
		return (size_t)((m_Scale.x / CELL_SIZE) * glm::floor(localPos.y / CELL_SIZE) + glm::floor(localPos.x / CELL_SIZE));
	}

}