#include <mcspch.hpp>
#include "GridMap.hpp"

// Temporary
#include "Frosty/API/AssetManager/AssetManager.hpp"

namespace MCS
{
	void GridMap::Init(const Frosty::ECS::CTransform& transform)
	{
		// Temporary
		auto& world = Frosty::Application::Get().GetWorld();

		//
		m_Position = transform.Position;
		m_Scale = transform.Scale;

		// Calculate and reserve space for amount of cells
		int totalCells = (int)(transform.Scale.x * transform.Scale.z / glm::pow((float)CELL_SIZE, 2));
		m_Cells.reserve(totalCells);


		for (int x = 1; x <= transform.Scale.x / CELL_SIZE; x++)
		{
			for (int z = 1; z <= transform.Scale.z / CELL_SIZE; z++)
			{
				m_Cells.emplace_back(Cell({ x * (CELL_SIZE / 2.0f) + (x - 1) * CELL_SIZE / 2.0f, z * (CELL_SIZE / 2.0f) + (z - 1) * CELL_SIZE / 2.0f }));
				//auto& cell = world->CreateEntity({ m_Position.x - m_Scale.x / 2.0f + m_Cells.back().Position.x, 0.01f, m_Position.z + m_Scale.z / 2.0f - m_Cells.back().Position.y }, { 0.0f, 0.0f, 0.0f }, { CELL_SIZE, 1.0f, CELL_SIZE }, true);
				//world->AddComponent<Frosty::ECS::CMesh>(cell, Frosty::AssetManager::GetMesh("pPlane1"));
				//auto& cellMat = world->AddComponent<Frosty::ECS::CMaterial>(cell, Frosty::AssetManager::GetShader("FlatColor"));
				//cellMat.Albedo = glm::vec4(m_Cells.back().Position.x / m_Scale.x, 0.0f, m_Cells.back().Position.y / m_Scale.z, 1.0f);
			}
		}

		//system("pause");
	}
}