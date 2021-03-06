#include <mcspch.hpp>
#include "Pathfinding.hpp"

namespace MCS
{
	void Pathfinding::Init(Frosty::Grid* grid)
	{
		m_Grid = grid;
	}

	glm::vec3 Pathfinding::FindPath(const glm::vec3& startPos, const glm::vec3& targetPos)
	{
		auto& startNode = m_Grid->WorldPointToNode(startPos);
		auto& targetNode = m_Grid->WorldPointToNode(targetPos);

		std::priority_queue<Frosty::CellNode> openSet;
		std::vector<Frosty::CellNode> closedSet;
		closedSet.reserve(50);
		openSet.push(startNode);
		Frosty::CellNode currentNode = openSet.top();

		while (!openSet.empty())
		{
			currentNode = openSet.top();
			m_Grid->GetNode(currentNode.GridX, currentNode.GridY)->ExistsInOpenSet = false;
			openSet.pop();
			closedSet.emplace_back(currentNode);
			//closedSet.insert(currentNode);

			if (currentNode == targetNode)
			{
				while (!openSet.empty())
				{
					Frosty::CellNode* tempNode = m_Grid->GetNode(openSet.top().GridX, openSet.top().GridY);
					tempNode->ExistsInOpenSet = false;
					tempNode->HCost = 0;
					tempNode->GCost = 0;
					openSet.pop();
				}
				while (!closedSet.empty())
				{
					Frosty::CellNode* tempNode = m_Grid->GetNode(closedSet.back().GridX, closedSet.back().GridY);
					tempNode->GCost = 0;
					tempNode->HCost = 0;
					closedSet.erase(closedSet.begin() + closedSet.size() - 1);
				}

				return RetracePath(&startNode, &targetNode);
			}

			for each (Frosty::CellNode* neighbour in m_Grid->GetNeighbours(&currentNode))
			{
				if ((neighbour->Walkable || *neighbour == targetNode) && !ExistsInClosedSet(closedSet, neighbour))
				{
					int32_t newMovementCostToNeightbour = currentNode.GCost + GetDistance(&currentNode, neighbour);
					if (newMovementCostToNeightbour < neighbour->GCost || !neighbour->ExistsInOpenSet)
					{
						neighbour->GCost = newMovementCostToNeightbour;
						neighbour->HCost = GetDistance(neighbour, &targetNode);
						neighbour->ParentGridX = currentNode.GridX;
						neighbour->ParentGridY = currentNode.GridY;

						if (*neighbour == startNode)
						{
							startNode = *neighbour;
						}
						if (*neighbour == targetNode)
						{
							targetNode = *neighbour;
						}

						if (!neighbour->ExistsInOpenSet)
						{
							neighbour->ExistsInOpenSet = true;
							openSet.push(*neighbour);
						}
					}
				}
			}
		}

		while (!openSet.empty())
		{
			Frosty::CellNode* tempNode = m_Grid->GetNode(openSet.top().GridX, openSet.top().GridY);
			tempNode->ExistsInOpenSet = false;
			tempNode->HCost = 0;
			tempNode->GCost = 0;
			openSet.pop();
		}
		while (!closedSet.empty())
		{
			//closedSet.back().GCost = 0;
			//closedSet.back().HCost = 0;
			//closedSet.erase(closedSet.begin() + closedSet.size() - 1);

			Frosty::CellNode* tempNode = m_Grid->GetNode(closedSet.back().GridX, closedSet.back().GridY);
			tempNode->GCost = 0;
			tempNode->HCost = 0;
			closedSet.erase(closedSet.begin() + closedSet.size() - 1);

		}

	//	m_Grid->CheckHCost();

		startNode.GCost = 0;
		startNode.HCost = 0;
		targetNode.GCost = 0;
		targetNode.HCost = 0;

		return glm::vec3(-1.0f);
	}

	int32_t Pathfinding::GetDistance(Frosty::CellNode* nodeA, Frosty::CellNode* nodeB) const
	{
		int32_t distX = glm::abs(nodeA->GridX - nodeB->GridX);
		int32_t distY = glm::abs(nodeA->GridY - nodeB->GridY);

		if (distX > distY)
		{
			return DIAGONAL_CELL_WEIGHT * distY + ADJACENT_CELL_WEIGHT * (distX - distY);
		}

		return DIAGONAL_CELL_WEIGHT * distX + ADJACENT_CELL_WEIGHT * (distY - distX);
	}

	glm::vec3 Pathfinding::RetracePath(Frosty::CellNode* startNode, Frosty::CellNode* targetNode)
	{
		if (*startNode == *targetNode) return targetNode->WorldPosition;

		std::vector<Frosty::CellNode*> path;
		Frosty::CellNode* currentNode = targetNode;
		currentNode->GCost = 0;
		currentNode->HCost = 0;


		while (*currentNode != *startNode)
		{
			path.emplace_back(currentNode);
			currentNode = m_Grid->GetNode(currentNode->ParentGridX, currentNode->ParentGridY);
			currentNode->GCost = 0;
			currentNode->HCost = 0;
		}

		std::reverse(path.begin(), path.end());

		m_Grid->DrawPathCells(path);

	//	m_Grid->CheckHCost();
		return path[0]->WorldPosition;
	}
	
	bool Pathfinding::ExistsInClosedSet(const std::vector<Frosty::CellNode>& closedSet, Frosty::CellNode* findNode)
	{
		for (size_t i = 0; i < closedSet.size(); i++)
		{
			if (closedSet[i] == *findNode) return true;
		}
		return false;
	}
}