#ifndef FORWARDPLUS_HPP
#define FORWARDPLUS_HPP

// THIS FILE TAKES CARE OF ALL THE NECESSARY PRE-CALCULATIONS FOR THE USE OF FORWARD PLUS

/*	1) Devide screen into a grid of 16x16
	2) Sort ligts depending on which screen they are intersecting with (in screen space)
	3) Sort the information in three buffers; 
	   array of all lights, array representing cells with an offset + nrOflights, array which holds the lights that the cells are refering to	*/

#include "Frosty/API/Lights/LightManager.hpp"
#include "Frosty/Core/Camera/Camera.hpp"

#include <vector>

namespace Frosty
{
	struct GridCell
	{
		float MinX, MaxX, MinY, MaxY;
	};

	class FrustumGrid
	{
	public:
		FrustumGrid();
		virtual ~FrustumGrid() {};
		FrustumGrid(const FrustumGrid& other);
		FrustumGrid& operator=(const FrustumGrid& other);

	private:
		// Forward Plus Calculations
		void CreateGridCells();												// Defines each cell						
		void LightCulling();
		bool CheckCollision(const GridCell& cell, const glm::vec3& light);	// AABB - Circle collision
		glm::vec3 ConvertToNDC(const glm::vec4& light, const glm::mat4& transform);		// Converts 3D positions to screen space, preferably 0-1

	private:

		static const uint8_t GRID_SIZE = 16;								// 16x16 grid, best suitable to our project
		
		std::array<GridCell, GRID_SIZE * GRID_SIZE> m_GridCells;			// Array of cells
		int m_TotalCells;
		
		LightManager m_LightManager;										// Contains an internal array of all point lights
		std::vector<PointLight> m_LightIndexList;							// Contains light indices, referring to the array of point lights in m_LightManager
		std::array<glm::vec2, GRID_SIZE * GRID_SIZE> m_CellLightsInfo;		// Contains an offset (x) to and size (y) of the light list for each tile
		

		std::unique_ptr<Camera> m_Camera;									// Needed for view and proj matrices
		glm::vec2 m_Frustum;												// Screen width (x) and height (y)

		static FrustumGrid* s_Instance;
	};
}
#endif