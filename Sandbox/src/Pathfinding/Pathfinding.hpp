#ifndef PATHFINDING_HPP
#define PATHFINDING_HPP

#include"Frosty/API/AssetManager/AssetFiles/Grid.hpp"

namespace MCS
{
	class Pathfinding
	{
	public:
		static const int ADJACENT_CELL_WEIGHT = 10;
		static const int DIAGONAL_CELL_WEIGHT = 14;

	public:
		Pathfinding() = default;
		virtual ~Pathfinding() = default;

		void Pathfinding::Init(Frosty::Grid* grid);
		glm::vec3 Pathfinding::FindPath(const glm::vec3& startPos, const glm::vec3& targetPos);

	private:
		int32_t Pathfinding::GetDistance(Frosty::CellNode* nodeA, Frosty::CellNode* nodeB) const;
		glm::vec3 Pathfinding::RetracePath(Frosty::CellNode* startNode, Frosty::CellNode* targetNode);
		bool ExistsInClosedSet(const std::vector<Frosty::CellNode>& closedSet, Frosty::CellNode* findNode);

	private:
		Frosty::Grid* m_Grid{ nullptr };

	};
}

#endif // !PATHFINDING_HPP
