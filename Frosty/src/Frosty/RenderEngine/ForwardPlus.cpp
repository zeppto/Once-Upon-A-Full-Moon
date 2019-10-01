#include "fypch.hpp"
#include "ForwardPlus.hpp"
#include "Frosty/Core/Application.hpp"

namespace Frosty
{
	FrustumGrid* FrustumGrid::s_Instance = nullptr;

	FrustumGrid::FrustumGrid()
	{
		s_Instance = this;

		m_TotalCells = 0;
		m_Camera.reset(FY_NEW Camera());
		m_Frustum = glm::vec2(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());

		CreateGridCells();
		LightCulling();

		//ConvertToNDC({ 3.f, 4.f, -5.f, 1.f });
		//CheckCollision(m_GridCells[223], ConvertToNDC({ 3.f, 2.f, -5.f, 1.f }));	// test values
	}

	FrustumGrid::FrustumGrid(const FrustumGrid& other)
	{
		//m_GridCells = other.m_GridCells;
		//m_TotalCells = other.m_TotalCells;
		//m_LightManager = other.m_LightManager;
		//m_LightIndexList = other.m_LightIndexList;
		//m_CellLightsInfo = other.m_CellLightsInfo;
		//m_Camera.reset(FY_NEW Camera());
		//m_Frustum = other.m_Frustum;
		
		FY_CORE_ASSERT(false, "Copy constructor is used in class 'FrustumGrid'.");
	}

	FrustumGrid& FrustumGrid::operator=(const FrustumGrid& other)
	{
		FY_CORE_ASSERT(false, "Assignment operator is used in class 'FrustumGrid'.");
		return *this;
	}

	void FrustumGrid::CreateGridCells()
	{
		float cellSizeX = m_Frustum.x / GRID_SIZE;
		float cellSizeY = m_Frustum.y / GRID_SIZE;

		float minX = 0, minY = 0;
		GridCell cell;

		for (int y = 0; y < GRID_SIZE; y++)
		{
			cell.MinY = minY;
			cell.MaxY = minY + cellSizeY;

			for (int x = 0; x < GRID_SIZE; x++)
			{
				cell.MinX = minX;
				cell.MaxX = minX + cellSizeX;

				m_GridCells[m_TotalCells++] = cell;
				
				minX += cellSizeX;
			}
			minX = 0;
			minY += cellSizeY;
		}
	}

	void FrustumGrid::LightCulling()
	{
		std::vector<int> pos;

		for (int i = 0; i < m_TotalCells; i++)
		{
			for (int j = 0; j < m_LightManager.GetAllPointLights().size(); j++)
			{
				if (CheckCollision(m_GridCells[i], ConvertToNDC(m_LightManager.GetPointLightAt(j)->GetPositionAndRadius(), m_LightManager.GetPointLightAt(j)->GetTransform())))
				{
					pos.emplace_back(i);
				}
			}
		}
	}

	bool FrustumGrid::CheckCollision(const GridCell& cell, const glm::vec3& light)	// light.xy = 2D position, light.z = radius
	{
		// Get center point circle first 
		glm::vec2 center = { light.x, light.y };
		
		// Calculate AABB info (center, half-extents)
		glm::vec2 aabb_half_extents(m_Frustum.x / GRID_SIZE / 2, m_Frustum.y / GRID_SIZE / 2);
		
		glm::vec2 aabb_center(
			cell.MinX + aabb_half_extents.x,
			cell.MinY + aabb_half_extents.y
		);

		// Get difference vector between both centers
		glm::vec2 difference = center - aabb_center;
		glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);	// A clamp operation clamps a value to a value within a given range
		
		// Add clamped value to AABB_center and we get the value of box closest to circle
		glm::vec2 closest = aabb_center + clamped;
		
		// Retrieve vector between center circle and closest point AABB and check if length <= radius
		difference = closest - center;
		
		return glm::length(difference) < light.z;
	}

	glm::vec3 FrustumGrid::ConvertToNDC(const glm::vec4& light, const glm::mat4& transform)	// light.xyz = 3D position, light.w = radius
	{
		// Converting from 3D space to NDC
		glm::mat4 VP = m_Camera.get()->GetProjection() * m_Camera.get()->GetView();
		glm::mat4 MVP = m_Camera.get()->GetProjection() * m_Camera.get()->GetView() * transform;

		glm::vec4 NDC = MVP * glm::vec4(glm::vec3(light.x, light.y, light.z), 1.f);
		NDC = NDC / NDC.z;

		// Converting from NDC to pixel
		int x = (int)round((NDC.x + 1.0f) * m_Frustum.x / 2.0f);
		int y = (int)round((NDC.y + 1.0f) * m_Frustum.y / 2.0f);


		// Calculate Radius and repeat process
		glm::vec4 NDC2 = MVP * glm::vec4(glm::vec3(light.x, light.y + light.w, light.z), 1.f);
		NDC2 = NDC2 / NDC2.z;

		int x2 = (int)round((NDC2.x + 1.0f) * m_Frustum.x / 2.0f);
		int y2 = (int)round((NDC2.y + 1.0f) * m_Frustum.y / 2.0f);

		glm::vec2 length = glm::vec2(x, y) - glm::vec2(x2, y2);
		float radius = glm::length(length);
		if (radius < 0)
			radius *= -1;

		return glm::vec3(x, y, radius);
	}
}