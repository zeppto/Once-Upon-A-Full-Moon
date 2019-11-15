#ifndef GRID_MAP_HPP
#define GRID_MAP_HPP


namespace MCS
{
	class GridMap
	{
	public:
		static const int CELL_SIZE = 1;
		static const int ADJACENT_CELL_WEIGHT = 10;
		static const int DIAGONAL_CELL_WEIGHT = 14;

		struct Cell 
		{
			glm::vec2 LocalPosition{ 0.0f };
			int16_t GCost{ 0 };										// Distance from origin node
			int16_t FCost{ std::numeric_limits<int16_t>::max() };	// Gcost + Hcost
			size_t Parent{ 0 };
			Frosty::ECS::EntityID OccupiedBy{ 0 };

			Cell() = default;
			Cell(const glm::vec2& localPos) : LocalPosition(localPos) { }

		};

	public:
		GridMap() = default;
		virtual ~GridMap();

		void Init(const Frosty::ECS::CTransform& mapTransform);

		glm::vec3 GetWorldPosition(const glm::vec2& localPos) const;
		glm::vec2 GetLocalPosition(const glm::vec3& worldPos) const;
		size_t GetCellIndex(const glm::vec2& localPos) const;
		size_t GetCellIndex(const glm::vec3& worldPos) const;
		const Cell& GetCell(size_t index) const { return *m_Cells[index]; }
		Cell& GetCell(size_t index) { return *m_Cells[index]; }
		glm::vec3 GetDestination(Frosty::ECS::EntityID entityID, const glm::vec3& origin, const glm::vec3& target);

		void UpdateOccupiedCells(Frosty::ECS::EntityID entityID, Frosty::ECS::CTransform* transform, Luna::BoundingBox* boundingBox);

		void ResetCellWeights();

	private:
		std::vector<size_t> GetNeighboringCells(size_t cellIndex);
		void OccupyCells(Frosty::ECS::EntityID entityID, const glm::vec4& collisionBox, int weight);
		int16_t GetDistanceWeight(size_t origin, size_t target);
		int16_t GetCellWeight(size_t current, size_t origin, size_t target);
		void SetCellWeight(size_t current, size_t origin, size_t target);
		size_t NodeWithLowestCost(std::vector<size_t>& openList, std::vector<size_t>& closedList, size_t targetIndex);
		bool ExistsInList(const std::vector<size_t>& cellList, size_t index) const;

	private:
		Frosty::World* m_World;
		glm::vec3 m_Position;		// This is position of the grid map in world space
		glm::vec3 m_Scale;
		std::vector<Cell*> m_Cells;
		std::vector<size_t> m_DynamicOccupiedCells;

	};
}

#endif // !GRID_MAP_HPP
