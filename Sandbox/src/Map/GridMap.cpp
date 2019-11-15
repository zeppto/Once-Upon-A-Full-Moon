#include <mcspch.hpp>
#include "GridMap.hpp"

// Temporary
#include "Frosty/API/AssetManager/AssetManager.hpp"

namespace MCS
{
	GridMap::~GridMap()
	{
		for (size_t i = 0; i < m_Cells.size(); i++)
		{
			delete m_Cells[i];
		}
	}

	void GridMap::Init(const Frosty::ECS::CTransform& mapTransform)
	{
		m_World = Frosty::Application::Get().GetWorld().get();
		auto& physicsCompData = m_World->GetComponentManager<Frosty::ECS::CPhysics>()->GetAll();
		size_t totalData = m_World->GetComponentManager<Frosty::ECS::CPhysics>()->GetTotal();
		
		// Check which entities should be included as obstacles 
		std::vector<Frosty::ECS::CTransform*> transforms;
		for (size_t i = 1; i < totalData; i++)
		{
			auto& tempTransform = m_World->GetComponent<Frosty::ECS::CTransform>(physicsCompData[i].EntityPtr);
			if (tempTransform.IsStatic)
			{
				// This means that the entity is static and has a CPhysics component
				transforms.emplace_back(&tempTransform);
			}
		}


		// Set map props
		m_Position = mapTransform.Position;
		m_Scale = mapTransform.Scale;

		// Calculate and reserve space for amount of cells
		size_t totalCells = (size_t)(m_Scale.x * m_Scale.z / glm::pow((float)CELL_SIZE, 2));
		m_Cells.reserve(totalCells);
		m_DynamicOccupiedCells.reserve((size_t)(totalCells * 0.1f));

		// Initiate cells
		for (int z = 1; z <= m_Scale.z / CELL_SIZE; z++)
		{
			for (int x = 1; x <= m_Scale.x / CELL_SIZE; x++)
			{
				glm::vec2 localCellPos = glm::vec2(x * (CELL_SIZE / 2.0f) + (x - 1) * CELL_SIZE / 2.0f, z * (CELL_SIZE / 2.0f) + (z - 1) * CELL_SIZE / 2.0f);
				m_Cells.emplace_back(FY_NEW Cell(localCellPos));
			}
		}

		// Set cell collision
		for (size_t i = 0; i < transforms.size(); i++)
		{
			auto& tempPhysics = m_World->GetComponent<Frosty::ECS::CPhysics>(transforms[i]->EntityPtr);
			glm::vec4 collisionBox = glm::vec4(
				transforms[i]->Position.x - transforms[i]->Scale.x * tempPhysics.BoundingBox->halfSize[0],
				transforms[i]->Position.z + transforms[i]->Scale.z * tempPhysics.BoundingBox->halfSize[2],
				transforms[i]->Position.x + transforms[i]->Scale.x * tempPhysics.BoundingBox->halfSize[0],
				transforms[i]->Position.z - transforms[i]->Scale.z * tempPhysics.BoundingBox->halfSize[2]
			);

			OccupyCells(tempPhysics.EntityPtr->Id, collisionBox, -2);
			
		}
		//FY_INFO("{0}", sizeof(Cell));

		//int cellColor = 0;
		//for (size_t i = 0; i < m_Cells.size(); i++)
		//{
		//	int r = ((int)glm::floor(i / (m_Scale.x / CELL_SIZE)) % 2) - (i % 2);
		//	cellColor = glm::abs(r);
		//	if (m_Cells[i].OccupiedBy == 0)
		//	{
		//		glm::vec3 worldPos = GetWorldPosition(m_Cells[i].LocalPosition);
		//		auto& cell = m_World->CreateEntity({ worldPos.x, 0.05f, worldPos.z }, { 0.0f, 0.0f, 0.0f }, { CELL_SIZE, 1.0f, CELL_SIZE }, true);
		//		m_World->AddComponent<Frosty::ECS::CMesh>(cell, Frosty::AssetManager::GetMesh("pPlane1"), m_Cells[i].FCost == -2 ? false : true);
		//		auto& cellMat = m_World->AddComponent<Frosty::ECS::CMaterial>(cell, Frosty::AssetManager::GetShader("FlatColor"));
		//		cellMat.Albedo = glm::vec4(cellColor, cellColor, cellColor, 1.0f);
		//		//cellMat.Albedo = glm::vec4(m_Cells[i].LocalPosition.x / m_Scale.x, 0.0f, m_Cells[i].LocalPosition.y / m_Scale.z, 1.0f);
		//		//m_Cells[i].CellEntityID = cell->Id;
		//	}
		//}
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

	size_t GridMap::GetCellIndex(const glm::vec3& worldPos) const
	{
		return GetCellIndex(GetLocalPosition(worldPos));
	}

	glm::vec3 GridMap::GetDestination(Frosty::ECS::EntityID entityID, const glm::vec3& origin, const glm::vec3& target)
	{
		// Check which cell is the origin and target
		size_t originCellIndex = GetCellIndex(origin);
		size_t targetCellIndex = GetCellIndex(target);

		if (originCellIndex == targetCellIndex)
		{
			return GetWorldPosition(m_Cells[originCellIndex]->LocalPosition);
		}

		// Create lists
		std::vector<size_t> openCells;
		std::vector<size_t> closedCells;

		// Add start to open list
		openCells.emplace_back(originCellIndex);
		size_t currentCell = openCells[0];
		
		while (currentCell != targetCellIndex && !openCells.empty())
		{
			// Set current node with the lowest F or H cost
			// Remove current from open	list and add it to the closed list
			currentCell = NodeWithLowestCost(openCells, closedCells, targetCellIndex);

			if (currentCell != targetCellIndex)
			{
				// Go through all the neighbors
				std::vector<size_t> neighboringCellIndices = GetNeighboringCells(currentCell);
				for (const auto& neighbor : neighboringCellIndices)
				{
					// Check if cell is walkable and is not in the closed list
					if ((m_Cells[neighbor]->OccupiedBy == entityID || m_Cells[neighbor]->OccupiedBy == 0 || neighbor == targetCellIndex) && !ExistsInList(closedCells, neighbor))
					{
						// Check if new path to the neighbor is shorter or the neighbor is not in the open list
						if (GetCellWeight(neighbor, currentCell, targetCellIndex) < m_Cells[neighbor]->FCost || !ExistsInList(openCells, neighbor))
						{
							// Update new cost, set parent and add cell to the open list if it is not there already
							SetCellWeight(neighbor, currentCell, targetCellIndex);
							m_Cells[neighbor]->Parent = currentCell;
							if (!ExistsInList(openCells, neighbor)) openCells.emplace_back(neighbor);
						}
					}
				}
			}
		}

		size_t currentIndex = targetCellIndex;
		size_t destinationIndex = m_Cells[currentIndex]->Parent;

		while (destinationIndex != originCellIndex)
		{
			currentIndex = destinationIndex;
			destinationIndex = m_Cells[currentIndex]->Parent;
		}

		return GetWorldPosition(m_Cells[currentIndex]->LocalPosition);
	}

	void GridMap::UpdateOccupiedCells(Frosty::ECS::EntityID entityID, Frosty::ECS::CTransform* transform, Luna::BoundingBox* boundingBox)
	{
		glm::vec4 collisionBox = glm::vec4(
			transform->Position.x - transform->Scale.x * boundingBox->halfSize[0],
			transform->Position.z + transform->Scale.z * boundingBox->halfSize[2],
			transform->Position.x + transform->Scale.x * boundingBox->halfSize[0],
			transform->Position.z - transform->Scale.z * boundingBox->halfSize[2]  
		);
		OccupyCells(entityID, collisionBox, -1);
	}

	void GridMap::ResetCellWeights()
	{
		for (size_t i = 0; i < m_DynamicOccupiedCells.size(); i++)
		{
			m_Cells[m_DynamicOccupiedCells[i]]->GCost = 0;
			m_Cells[m_DynamicOccupiedCells[i]]->FCost = std::numeric_limits<int16_t>::max();
			m_Cells[m_DynamicOccupiedCells[i]]->OccupiedBy = 0;
			//auto& currentCellEntity = m_World->GetEntityManager()->GetEntityById(m_Cells[i].CellEntityID);
			//m_World->GetComponent<Frosty::ECS::CMesh>(currentCellEntity).RenderMesh = true;
		}

		m_DynamicOccupiedCells.clear();
	}

	std::vector<size_t> GridMap::GetNeighboringCells(size_t cellIndex)
	{
		// Initiate
		std::vector<size_t> neighbors;
		neighbors.reserve(8);

		// Calculate neighboring cells and check if they're valid
		// Right
		size_t neighborIndex = cellIndex + 1;
		if (neighborIndex >= 0 && neighborIndex < m_Cells.size())
		{
			if (m_Cells[neighborIndex] != nullptr)
			{
				if (m_Cells[neighborIndex]->LocalPosition.y == m_Cells[cellIndex]->LocalPosition.y)
				{
					neighbors.emplace_back(neighborIndex);
				}
			}
		}
		// Left
		neighborIndex = cellIndex - 1;
		if (neighborIndex >= 0 && neighborIndex < m_Cells.size())
		{
			if (m_Cells[neighborIndex] != nullptr)
			{
				if (m_Cells[neighborIndex]->LocalPosition.y == m_Cells[cellIndex]->LocalPosition.y)
				{
					neighbors.emplace_back(neighborIndex);
				}
			}
		}
		// Top
		neighborIndex = cellIndex + (size_t)(m_Scale.x / CELL_SIZE);
		if (neighborIndex >= 0 && neighborIndex < m_Cells.size())
		{
			if (m_Cells[neighborIndex] != nullptr)
			{
				if (m_Cells[neighborIndex]->LocalPosition.x == m_Cells[cellIndex]->LocalPosition.x)
				{
					neighbors.emplace_back(neighborIndex);
				}
			}
		}
		// Bot
		neighborIndex = cellIndex - (size_t)(m_Scale.x / CELL_SIZE);
		if (neighborIndex >= 0 && neighborIndex < m_Cells.size())
		{
			if (m_Cells[neighborIndex] != nullptr)
			{
				if (m_Cells[neighborIndex]->LocalPosition.x == m_Cells[cellIndex]->LocalPosition.x)
				{
					neighbors.emplace_back(neighborIndex);
				}
			}
		}
		// Top Right
		neighborIndex = cellIndex + (size_t)(m_Scale.x / CELL_SIZE) + 1;
		if (neighborIndex >= 0 && neighborIndex < m_Cells.size())
		{
			if (m_Cells[neighborIndex] != nullptr)
			{
				if (m_Cells[neighborIndex]->LocalPosition.y == (m_Cells[cellIndex]->LocalPosition.y + CELL_SIZE) &&
					m_Cells[neighborIndex]->LocalPosition.x == (m_Cells[cellIndex]->LocalPosition.x + CELL_SIZE))
				{
					neighbors.emplace_back(neighborIndex);
				}
			}
		}
		// Top Left
		neighborIndex = cellIndex + (size_t)(m_Scale.x / CELL_SIZE) - 1;
		if (neighborIndex >= 0 && neighborIndex < m_Cells.size())
		{
			if (m_Cells[neighborIndex] != nullptr)
			{
				if (m_Cells[neighborIndex]->LocalPosition.y == (m_Cells[cellIndex]->LocalPosition.y + CELL_SIZE) &&
					m_Cells[neighborIndex]->LocalPosition.x == (m_Cells[cellIndex]->LocalPosition.x - CELL_SIZE))
				{
					neighbors.emplace_back(neighborIndex);
				}
			}
		}
		// Bot Right
		neighborIndex = cellIndex - (size_t)(m_Scale.x / CELL_SIZE) + 1;
		if (neighborIndex >= 0 && neighborIndex < m_Cells.size())
		{
			if (m_Cells[neighborIndex] != nullptr)
			{
				if (m_Cells[neighborIndex]->LocalPosition.y == (m_Cells[cellIndex]->LocalPosition.y - CELL_SIZE) &&
					m_Cells[neighborIndex]->LocalPosition.x == (m_Cells[cellIndex]->LocalPosition.x + CELL_SIZE))
				{
					neighbors.emplace_back(neighborIndex);
				}
			}
		}
		// Bot Left
		neighborIndex = cellIndex - (size_t)(m_Scale.x / CELL_SIZE) - 1;
		if (neighborIndex >= 0 && neighborIndex < m_Cells.size())
		{
			if (m_Cells[neighborIndex] != nullptr)
			{
				if (m_Cells[neighborIndex]->LocalPosition.y == (m_Cells[cellIndex]->LocalPosition.y - CELL_SIZE) &&
					m_Cells[neighborIndex]->LocalPosition.x == (m_Cells[cellIndex]->LocalPosition.x - CELL_SIZE))
				{
					neighbors.emplace_back(neighborIndex);
				}
			}
		}

		return neighbors;
	}

	void GridMap::OccupyCells(Frosty::ECS::EntityID entityID, const glm::vec4& collisionBox, int weight)
	{
		glm::vec2 localPosBL = GetLocalPosition({ collisionBox.x, 0.0f, collisionBox.y });
		glm::vec2 localPosTR = GetLocalPosition({ collisionBox.z, 0.0f, collisionBox.w });
		glm::vec2 localPosBR = GetLocalPosition({ collisionBox.z, 0.0f, collisionBox.y });

		size_t cellIndexBL = GetCellIndex(localPosBL);
		size_t cellIndexTR = GetCellIndex(localPosTR);
		size_t cellIndexBR = GetCellIndex(localPosBR);



		size_t cellsInZ = (size_t)(glm::floor(cellIndexTR / (m_Scale.z / CELL_SIZE)) - glm::floor(cellIndexBL / (m_Scale.z / CELL_SIZE)));
		size_t cellsInX = cellIndexBR - cellIndexBL;

		for (size_t z = 0; z <= cellsInZ; z++)
		{
			size_t cellIndexStart = cellIndexBL + (size_t)(m_Scale.x / CELL_SIZE) * z;
			for (size_t x = 0; x <= cellsInX; x++)
			{
				if (weight == -1)
				{
					m_DynamicOccupiedCells.emplace_back(cellIndexStart + x);
					m_Cells[cellIndexStart + x]->FCost = weight;
					m_Cells[cellIndexStart + x]->OccupiedBy = entityID;
				}
			}
		}
	}

	int16_t GridMap::GetDistanceWeight(size_t origin, size_t target)
	{
		float originCellInfo = (float)origin / (m_Scale.x / CELL_SIZE);
		float targetCellInfo = (float)target / (m_Scale.x / CELL_SIZE);
		int originCellRow = (int)glm::floor(originCellInfo);
		int targetCellRow = (int)glm::floor(targetCellInfo);
		float originCellCol = originCellInfo - originCellRow;
		float targetCellCol = targetCellInfo - targetCellRow;
		int rowDistance = (int)glm::abs(originCellRow - targetCellRow);
		int colDistance = (int)glm::round(glm::abs(originCellCol - targetCellCol) * (m_Scale.x / (float)CELL_SIZE));
		int nrOfAdjacents = glm::abs(rowDistance - colDistance);
		int nrOfDiagonals = glm::max(rowDistance, colDistance) - nrOfAdjacents;

		return nrOfAdjacents * ADJACENT_CELL_WEIGHT + nrOfDiagonals * DIAGONAL_CELL_WEIGHT;
	}

	int16_t GridMap::GetCellWeight(size_t current, size_t origin, size_t target)
	{
		if (current >= 0 && current < m_Cells.size())
		{
			return GetDistanceWeight(origin, current) + GetDistanceWeight(target, current) + m_Cells[origin]->GCost;
		}
		else
		{
			return -1;
		}
	}

	void GridMap::SetCellWeight(size_t current, size_t origin, size_t target)
	{
		if (current >= 0 && current < m_Cells.size())
		{
			m_Cells[current]->GCost = GetDistanceWeight(origin, current) + m_Cells[origin]->GCost;
			//m_Cells[current].HCost = GetDistanceWeight(current, target);
			int16_t totalGCost = m_Cells[current]->GCost + GetDistanceWeight(current, target);
			m_Cells[current]->FCost = glm::min(m_Cells[current]->FCost, totalGCost);
		}
	}

	size_t GridMap::NodeWithLowestCost(std::vector<size_t>& openList, std::vector<size_t>& closedList, size_t targetIndex)
	{
		size_t openListIndex = 0;
		size_t lowestCellIndex = openList.at(0);

		// Find lowest cost
		for (size_t i = 1; i < openList.size(); i++)
		{
			if (m_Cells[lowestCellIndex]->FCost > m_Cells[openList.at(i)]->FCost)
			{
				lowestCellIndex = openList.at(i);
				openListIndex = i;
			}
			else if (m_Cells[lowestCellIndex]->FCost == m_Cells[openList.at(i)]->FCost)
			{
				if (GetDistanceWeight(lowestCellIndex, targetIndex) > GetDistanceWeight(openList.at(i), targetIndex))
				//if (m_Cells[lowestCellIndex].HCost > m_Cells[openList.at(i)].HCost)
				{
					lowestCellIndex = openList.at(i);
					openListIndex = i;
				}
			}
		}

		// Remove from open list
		openList.erase(openList.begin() + openListIndex);

		// Add to closed list
		closedList.emplace_back(lowestCellIndex);

		return lowestCellIndex;
	}

	bool GridMap::ExistsInList(const std::vector<size_t>& cellList, size_t index) const
	{
		for (const auto& cell : cellList)
		{
			if (index == cell)
			{
				return true;
			}
		}
		return false;
	}
}