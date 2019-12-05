#include <mcspch.hpp>
#include "Grid.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"

namespace MCS
{
	void Grid::Init(const Frosty::ECS::CTransform& mapTransform)
	{
		m_World = Frosty::Application::Get().GetWorld().get();
		m_GridWorldPosition = mapTransform.Position;
		m_GridWorldSize = glm::vec2(mapTransform.Scale.x, mapTransform.Scale.z);
		m_GridSize = glm::vec2(m_GridWorldSize.x / CELL_SIZE, m_GridWorldSize.y / CELL_SIZE);
		CreateGrid();
		m_PathNodes.reserve(50);
	}

	void Grid::Reset()
	{
		for (size_t i = 0; i < m_DynamicOccupiedNodes.size(); i++)
		{
			if (m_DrawGizmos)
			{
				auto& cellMat = m_World->GetComponent<Frosty::ECS::CMaterial>(m_World->GetEntityManager()->GetEntityById(m_DynamicOccupiedNodes[i]->CellEntityID));
				cellMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("green_square");
			}
			m_DynamicOccupiedNodes[i]->Walkable = true;
			m_DynamicOccupiedNodes[i] = nullptr;
		}
		m_DynamicOccupiedNodes.clear();

		if (!m_DrawGizmos) return;

		for (size_t i = 0; i < m_PathNodes.size(); i++)
		{
			auto& cellMat = m_World->GetComponent<Frosty::ECS::CMaterial>(m_World->GetEntityManager()->GetEntityById(m_PathNodes[i].CellEntityID));
			cellMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("green_square");
		}
		m_PathNodes.clear();
	}

	std::vector<CellNode*> Grid::GetNeighbours(CellNode* cellNode)
	{
		std::vector<CellNode*> neighbours;

		for (int32_t x = -1; x <= 1; x++)
		{
			for (int32_t y = -1; y <= 1; y++)
			{
				if (x != 0 || y != 0)
				{
					int32_t checkX = cellNode->GridX + x;
					int32_t checkY = cellNode->GridY + y;

					if (checkX >= 0 && checkX < m_GridSize.x && checkY >= 0 && checkY < m_GridSize.y)
					{
						int32_t nodeIndex = checkY * (int32_t)m_GridSize.x + checkX;
						neighbours.emplace_back(&m_CellNodes[nodeIndex]);
					}
				}
			}
		}

		return neighbours;
	}

	CellNode* Grid::GetNode(int32_t gridX, int32_t gridY)
	{
		int32_t index = gridY * (int32_t)m_GridSize.x + gridX;
		return &m_CellNodes[index];
	}
	
	CellNode& Grid::WorldPointToNode(const glm::vec3& worldPoint)
	{
		glm::vec2 percentage = glm::vec2((worldPoint.x + m_GridWorldSize.x * 0.5f - m_GridWorldPosition.x) / m_GridWorldSize.x, (worldPoint.z + m_GridWorldSize.y * 0.5f - m_GridWorldPosition.z) / m_GridWorldSize.y);
		percentage = glm::clamp(percentage, { 0.01f }, { 0.99f });

		uint32_t col = (int)glm::floor(m_GridSize.x * percentage.x);
		uint32_t row = (int)glm::floor(m_GridSize.y - m_GridSize.y * percentage.y);
		uint32_t index = (int)(row * m_GridSize.x) + col;

		return m_CellNodes[index];
	}

	void Grid::SetNodeUnwalkable(const glm::vec3& worldPoint)
	{
		auto& cellNode = WorldPointToNode(worldPoint);
		cellNode.Walkable = false;
		m_DynamicOccupiedNodes.emplace_back(&cellNode);
	}

	void Grid::DrawTargetCell(Frosty::ECS::CTransform* transform)
	{
		if (!m_DrawGizmos) return;

		auto& player = m_World->GetComponentManager<Frosty::ECS::CPlayer>()->GetAll();
		auto& playerTransform = m_World->GetComponent<Frosty::ECS::CTransform>(player[1].EntityPtr);
		CellNode& playerNode = WorldPointToNode(playerTransform.Position);
		
		auto& cellMat = m_World->GetComponent<Frosty::ECS::CMaterial>(m_World->GetEntityManager()->GetEntityById(playerNode.CellEntityID));
		cellMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("blue_square");
	}

	void Grid::DrawSeekerCell(Frosty::ECS::CTransform* transform)
	{
		if (!m_DrawGizmos) return;

		CellNode& enemyNode = WorldPointToNode(transform->Position);

		//if (enemyNode.Walkable)
		{
			auto& cellMat = m_World->GetComponent<Frosty::ECS::CMaterial>(m_World->GetEntityManager()->GetEntityById(enemyNode.CellEntityID));
			cellMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("purple_square");
			
		}
	}

	void Grid::DrawPathCells(const std::vector<CellNode*> path)
	{
		if (!m_DrawGizmos) return;

		for (size_t i = 0; i < path.size(); i++)
		{
			auto& cellMat = m_World->GetComponent<Frosty::ECS::CMaterial>(m_World->GetEntityManager()->GetEntityById(path[i]->CellEntityID));
			cellMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("brown_square");
			m_PathNodes.emplace_back(*path[i]);
		}
	}

	void Grid::CreateGrid()
	{
		m_CellNodes.reserve((size_t)(m_GridSize.x) * (size_t)(m_GridSize.y));
		glm::vec3 worldBottomLeft = m_GridWorldPosition - glm::vec3(1.0f, 0.0f, 0.0f) * m_GridWorldSize.x / 2.0f + glm::vec3(0.0f, 0.0f, 1.0f) * m_GridWorldSize.y / 2.0f;
		glm::vec3 worldPoint = glm::vec3(0.0f);
		bool walkable = true;

		for (int32_t y = 0; y < m_GridSize.y; y++)
		{
			for (int32_t x = 0; x < m_GridSize.x; x++)
			{
				worldPoint = worldBottomLeft + glm::vec3(1.0f, 0.0f, 0.0f) * (x * CELL_SIZE + CELL_SIZE * 0.5f) - glm::vec3(0.0f, 0.0f, 1.0f) * (y * CELL_SIZE + CELL_SIZE * 0.5f);
				walkable = !CheckCollision(worldPoint, CELL_SIZE * 0.25f);
				m_CellNodes.emplace_back(worldPoint, walkable, x, y);
			}
		}

		if (!m_DrawGizmos) return;

		for (size_t i = 0; i < m_CellNodes.size(); i++)
		{
			auto& cell = m_World->CreateEntity({ m_CellNodes[i].WorldPosition.x, 0.05, m_CellNodes[i].WorldPosition.z }, { 0.0f,0.0f,0.0f }, { CELL_SIZE, 1.0f, CELL_SIZE }, true);
			cell->ShowInEditor = false;
			m_World->AddComponent<Frosty::ECS::CMesh>(cell, Frosty::AssetManager::GetMesh("pPlane1"));
			auto& cellMat = m_World->AddComponent<Frosty::ECS::CMaterial>(cell, Frosty::AssetManager::GetShader("Texture2D"));
			cellMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D(m_CellNodes[i].Walkable ? "green_square" : "red_square");
			m_CellNodes[i].CellEntityID = cell->Id;
		}
	}

	bool Grid::CheckCollision(const glm::vec3& worldPoint, float radius) const
	{
		// Collision
		auto& physicsCompData = m_World->GetComponentManager<Frosty::ECS::CPhysics>()->GetAll();
		size_t totalData = m_World->GetComponentManager<Frosty::ECS::CPhysics>()->GetTotal();

		// Check which entities should be included as obstacles
		for (size_t i = 1; i < totalData; i++)
		{
			auto& tempTransform = m_World->GetComponent<Frosty::ECS::CTransform>(physicsCompData[i].EntityPtr);
			//if (tempTransform.IsStatic)
			if (!m_World->HasComponent<Frosty::ECS::CEnemy>(tempTransform.EntityPtr) && !m_World->HasComponent<Frosty::ECS::CPlayer>(tempTransform.EntityPtr))
			{
				// This means that the entity is static and has a CPhysics component
				auto& tempPhysics = m_World->GetComponent<Frosty::ECS::CPhysics>(tempTransform.EntityPtr);
				glm::vec4 collisionBox = glm::vec4(
					tempTransform.Position.x - tempTransform.Scale.x * tempPhysics.BoundingBox->halfSize[0] - radius,
					tempTransform.Position.z + tempTransform.Scale.z * tempPhysics.BoundingBox->halfSize[2] + radius,
					tempTransform.Position.x + tempTransform.Scale.x * tempPhysics.BoundingBox->halfSize[0] + radius,
					tempTransform.Position.z - tempTransform.Scale.z * tempPhysics.BoundingBox->halfSize[2] - radius
				);

				if (worldPoint.x > collisionBox.x && worldPoint.x < collisionBox.z &&
					worldPoint.z < collisionBox.y && worldPoint.z > collisionBox.w)
				{
					return true;
				}
			}
		}

		return false;
	}
}