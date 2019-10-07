#ifndef FORWARDPLUS_HPP
#define FORWARDPLUS_HPP

// THIS FILE TAKES CARE OF ALL THE NECESSARY PRE-CALCULATIONS FOR THE USE OF FORWARD PLUS

/*	1) Devide screen into a grid of 16x16
	2) Check to see if light is in the frustum (in screen space)
	3) Sort the information in three buffers; 
	   array of all lights, array representing cells with an offset + nrOflights, array which holds the lights that the cells are refering to	*/

#include "Frosty/API/Lights/LightManager.hpp"
#include "Frosty/Core/Camera/Camera.hpp"

namespace Frosty
{
	struct GridCell							// Corners of a cell
	{
		float MinX, MaxX, MinY, MaxY;
	};

	struct CellLightinfo					// Information that m_CellLightInfo stores
	{
		uint16_t Offset = 0;
		uint16_t Size = 0;
	};

	class FrustumGrid
	{
	private:
		static const uint8_t GRID_SIZE = 16;								// 16x16 grid, best suitable to our project

	public:
		FrustumGrid();
		virtual ~FrustumGrid() {};
		FrustumGrid(const FrustumGrid& other);
		FrustumGrid& operator=(const FrustumGrid& other);

		void Update();
		void Initiate(std::shared_ptr<LightManager>& lightManager);

		// Get
		const int GetNrOfGrids();
		const std::vector<int>& GetLightIndexList();
		const glm::vec2 GetCellLightInfoAt(int position);

	private:
		// Forward Plus Calculations
		void CreateGridCells();												// Defines each cell						
		void LightCulling();												// Here is where all the sorting happens
		bool CheckCollision(const GridCell& cell, const glm::vec3& light);	// AABB - Circle collision
		glm::vec3 ConvertToNDC(const glm::vec4& light, const glm::mat4& transform);		// Converts 3D positions to screen space, preferably 0-1

	private:
		std::array<GridCell, GRID_SIZE * GRID_SIZE> m_GridCells;			// Array of cells
		int m_TotalCells;
		
		std::shared_ptr<LightManager> m_LightManager;						// Contains an internal array of all point lights
		std::vector<int> m_LightIndexList;									// Contains light indices, referring to the array of point lights in m_LightManager
		std::array<CellLightinfo, GRID_SIZE * GRID_SIZE> m_CellLightsInfo;	// Contains an offset (x) to and size (y) of the light list for each tile
		

		std::unique_ptr<Camera> m_Camera;									// Needed for view and proj matrices
		glm::vec2 m_Frustum;												// Screen width (x) and height (y)

		static FrustumGrid* s_Instance;
	};
}
#endif