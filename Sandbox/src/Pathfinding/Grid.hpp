#ifndef GRID_HPP
#define GRID_HPP

namespace MCS
{
	struct CellNode
	{
		glm::vec3 WorldPosition{ 0.0f };
		bool Walkable{ true };
		int32_t GridX{ -1 };
		int32_t GridY{ -1 };

		int16_t GCost{ 0 };
		int16_t HCost{ 0 };
		int16_t ExtraCost{ 0 };
		int32_t ParentGridX{ -1 };
		int32_t ParentGridY{ -1 };
		bool ExistsInOpenSet{ false };


		// TEMPORARY FOR DEBUGGING
		Frosty::ECS::EntityID CellEntityID{ 0 };

		CellNode(const glm::vec3& worldPos, bool walkable, int32_t gridX, int32_t gridY) : WorldPosition(worldPos), Walkable(walkable), GridX(gridX), GridY(gridY) { }
		
		// Operators
		bool operator==(const CellNode& obj) const { return obj.GridX == GridX && obj.GridY == GridY; }
		bool operator!=(const CellNode& obj) const { return !(obj.GridX == GridX && obj.GridY == GridY); }
		bool operator<(const CellNode& obj) const { return GetFCost() > obj.GetFCost() || GetFCost() == obj.GetFCost() && HCost > obj.HCost; }

		int32_t GetFCost() const { return GCost + HCost; }
	};


	class Grid
	{
	public:
		static const int CELL_SIZE = 4;

	public:
		Grid() = default;
		virtual ~Grid() = default;

		void Init(const Frosty::ECS::CTransform& mapTransform);

		void Reset();

		std::vector<CellNode*> GetNeighbours(CellNode* cellNode);
		CellNode* GetNode(int32_t gridX, int32_t gridY);

		CellNode& WorldPointToNode(const glm::vec3& worldPoint);
		void SetNodeUnwalkable(const glm::vec3& worldPoint);

		// TEMPORARY FOR DEUGGING
		void DrawTargetCell(Frosty::ECS::CTransform* transform);
		void DrawSeekerCell(Frosty::ECS::CTransform* transform);
		void DrawPathCells(const std::vector<CellNode*> path);

	private:
		void CreateGrid();
		bool CheckCollision(const glm::vec3& worldPoint, float radius) const;

	private:
		Frosty::World* m_World;
		glm::vec3 m_GridWorldPosition{ 0.0f };		// Position(XZ) of the plane in world space 
		glm::vec2 m_GridWorldSize{ 0.0f };			// Scale(XZ) of the plane in world space
		glm::vec2 m_GridSize{ 0.0f };				// Number of grids in XZ

		std::vector<CellNode> m_CellNodes;
		std::vector<CellNode*> m_DynamicOccupiedNodes;

		// TEMPORARY FOR DEUGGING
		bool m_DrawGizmos{ false };
		std::vector<CellNode> m_PathNodes;
	};
}

namespace std
{
	template <>
	struct hash<MCS::CellNode>
	{
		size_t operator()(const MCS::CellNode& k) const
		{
			// Compute individual hash values for two data members and combine them using XOR and bit shifting
			return ((hash<int>()(k.GridX) ^ (hash<int>()(k.GridY) << 1)) >> 1);
		}
	};
}

#endif // !GRID_HPP
