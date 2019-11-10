#ifndef GRID_MAP_HPP
#define GRID_MAP_HPP


namespace MCS
{
	class GridMap
	{
	public:
		static const int CELL_SIZE = 2;
		struct Cell 
		{
			glm::vec2 LocalPosition{ 0.0f };
			glm::vec3 WorldPosition{ 0.0f };
			int weight{ 0 };

			Cell() = default;
			Cell(const glm::vec2& localPos, const glm::vec3& worldPos) : LocalPosition(localPos), WorldPosition(worldPos) { }
		};

	public:
		GridMap() = default;
		virtual ~GridMap() = default;

		void Init(const Frosty::ECS::CTransform& transform);

		glm::vec3 GetWorldPosition(const glm::vec2& localPos) const;
		glm::vec2 GetLocalPosition(const glm::vec3& worldPos) const;
		size_t GetCellIndex(const glm::vec2& localPos) const;

	private:
		glm::vec3 m_Position;		// This is position of the grid map in world space
		glm::vec3 m_Scale;
		std::vector<Cell> m_Cells;

	};
}

#endif // !GRID_MAP_HPP
