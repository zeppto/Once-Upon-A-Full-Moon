#ifndef GRID_HPP
#define GRID_HPP


namespace Frosty
{
	class World;
	namespace ECS { struct CTransform; }

	struct CellNode
	{
		glm::vec3 WorldPosition{ 0.0f };
		bool Walkable{ true };
		int32_t GridX{ -1 };
		int32_t GridY{ -1 };

		int16_t GCost{ 0 };
		int16_t HCost{ 0 };
		int32_t ParentGridX{ -1 };
		int32_t ParentGridY{ -1 };
		bool ExistsInOpenSet{ false };


		// TEMPORARY FOR DEBUGGING
		size_t CellEntityID{ 0 };

		inline CellNode() {};
		inline CellNode(const glm::vec3& worldPos, bool walkable, int32_t gridX, int32_t gridY) : WorldPosition(worldPos), Walkable(walkable), GridX(gridX), GridY(gridY) { }
		
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



		void SaveFile(const std::string FileName);
		void LoadFile(const std::string FilePath);
		inline void LoadFile() {
			if (m_FilePath != "") { LoadFile(m_FilePath); }
			else { FY_CORE_ASSERT(0, "Grid does not have a filepath!"); }
		}

		inline void SetFileName(const std::string& FileName) { m_FileName = FileName; }
		inline void SetFilePath(const std::string& FilePath) { m_FilePath = FilePath; }

	private:
		void CreateGrid();
		bool CheckCollision(const glm::vec3& worldPoint, float radius) const;

	private:
		World* m_World;
		glm::vec3 m_GridWorldPosition{ 0.0f };		// Position(XZ) of the plane in world space 
		glm::vec2 m_GridWorldSize{ 0.0f };			// Scale(XZ) of the plane in world space
		glm::vec2 m_GridSize{ 0.0f };				// Number of grids in XZ

		std::vector<CellNode> m_CellNodes;
		std::vector<CellNode*> m_DynamicOccupiedNodes;

		std::string m_FileName{ "" };
		std::string m_FilePath{ "" };

		// TEMPORARY FOR DEUGGING
		bool m_DrawGizmos{ false };
		std::vector<CellNode> m_PathNodes;
	};
}

namespace std
{
	template <>
	struct hash<Frosty::CellNode>
	{
		size_t operator()(const Frosty::CellNode& k) const
		{
			// Compute individual hash values for two data members and combine them using XOR and bit shifting
			return ((hash<int>()(k.GridX) ^ (hash<int>()(k.GridY) << 1)) >> 1);
		}
	};
}

#endif // !GRID_HPP
