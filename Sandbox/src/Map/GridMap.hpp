#ifndef GRID_MAP_HPP
#define GRID_MAP_HPP


namespace MCS
{
	class GridMap
	{
	public:
		static const int CELL_SIZE = 5;
		struct Cell 
		{
			glm::vec2 Position{ 0.0f };

			Cell() = default;
			Cell(const glm::vec2& pos) : Position(pos) { }
		};

	public:
		GridMap() = default;
		virtual ~GridMap() = default;

		void Init(const Frosty::ECS::CTransform& transform);

	private:
		glm::vec3 m_Position;		// This is position of the grid map in world space
		glm::vec3 m_Scale;
		std::vector<Cell> m_Cells;

	};
}

#endif // !GRID_MAP_HPP
