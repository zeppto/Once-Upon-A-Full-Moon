#include<fypch.hpp>
//#include "Grid.hpp"
//#include"Frosty/API/AssetManager/AssetFiles/Grid.hpp"
//#include "Frosty/API/AssetManager/AssetManager.hpp"
//#include"Frosty/Core/Application.hpp"
//#include"Frosty/Core/ECS.hpp"

#include "Frosty/API/AssetManager/AssetManager.hpp"
#include<fstream>
#include<ostream>
#include<istream>
//#include<Frosty/Core/ECS.hpp>
#include"Frosty/Core/Application.hpp"
//#include "Frosty/Core/World.hpp"

namespace Frosty
{
	void Grid::Init(const ECS::CTransform& mapTransform)
	{
		m_World = Application::Get().GetWorld().get();
		m_GridWorldPosition = mapTransform.Position;
		m_GridWorldSize = glm::vec2(mapTransform.Scale.x, mapTransform.Scale.z);
		m_GridSize = glm::vec2(m_GridWorldSize.x / CELL_SIZE, m_GridWorldSize.y / CELL_SIZE);
		CreateGrid();
		m_PathNodes.reserve(50);
	}

	void Grid::Reset()
	{
		if (!m_DrawGizmos) return;

		for (size_t i = 0; i < m_DynamicOccupiedNodes.size(); i++)
		{
			auto& cellMat = m_World->GetComponent<Frosty::ECS::CMaterial>(m_World->GetEntityManager()->GetEntityById(m_DynamicOccupiedNodes[i]->CellEntityID));
			cellMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("green_square");
			m_DynamicOccupiedNodes[i]->Walkable = true;
			m_DynamicOccupiedNodes[i] = nullptr;
		}
		m_DynamicOccupiedNodes.clear();

		for (size_t i = 0; i < m_PathNodes.size(); i++)
		{
			auto& cellMat = m_World->GetComponent<Frosty::ECS::CMaterial>(m_World->GetEntityManager()->GetEntityById(m_PathNodes[i].CellEntityID));
			cellMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("green_square");
		}
		m_PathNodes.clear();
	}

	std::vector<CellNode*> Grid::GetNeighbours(CellNode* cellNode)
	{
		std::vector<CellNode*> neighbours;

		for (int32_t x = -1; x <= 1; x++)
		{
			for (int32_t y = -1; y <= 1; y++)
			{
				if (x != 0 || y != 0)
				{
					int32_t checkX = cellNode->GridX + x;
					int32_t checkY = cellNode->GridY + y;

					if (checkX >= 0 && checkX < m_GridSize.x && checkY >= 0 && checkY < m_GridSize.y)
					{
						int32_t nodeIndex = checkY * (int32_t)m_GridSize.x + checkX;
						neighbours.emplace_back(&m_CellNodes[nodeIndex]);
					}
				}
			}
		}

		return neighbours;
	}

	CellNode* Grid::GetNode(int32_t gridX, int32_t gridY)
	{
		int32_t index = gridY * (int32_t)m_GridSize.x + gridX;
		return &m_CellNodes[index];
	}
	
	CellNode& Grid::WorldPointToNode(const glm::vec3& worldPoint)
	{
		//glm::vec2 percentage = glm::vec2((worldPoint.x - m_GridWorldSize.x + m_GridWorldSize.x * 0.5f) / m_GridWorldSize.x, (worldPoint.z + m_GridWorldSize.y * 0.5f) / m_GridWorldSize.y);

		glm::vec2 percentage = glm::vec2(
			(worldPoint.x + m_GridWorldSize.x * 0.5f - m_GridWorldPosition.x) / m_GridWorldSize.x,
			(worldPoint.z + m_GridWorldSize.y * 0.5f - m_GridWorldPosition.z) / m_GridWorldSize.y);


		percentage = glm::clamp(percentage, { 0.01f }, { 0.99f });
		
		
		uint32_t col = (int)glm::floor(m_GridSize.x * percentage.x);
		uint32_t row = (int)glm::floor(m_GridSize.y - m_GridSize.y * percentage.y);
		uint32_t index = (int)(row * m_GridSize.x) + col;

		return m_CellNodes[index];
	}

	void Grid::SetNodeUnwalkable(const glm::vec3& worldPoint)
	{
		WorldPointToNode(worldPoint).Walkable = false;
	}

	void Grid::DrawTargetCell(Frosty::ECS::CTransform* transform)
	{
		if (!m_DrawGizmos) return;

		auto& player = m_World->GetComponentManager<Frosty::ECS::CPlayer>()->GetAll();
		auto& playerTransform = m_World->GetComponent<Frosty::ECS::CTransform>(player[1].EntityPtr);
		CellNode& playerNode = WorldPointToNode(playerTransform.Position);
		
		auto& cellMat = m_World->GetComponent<Frosty::ECS::CMaterial>(m_World->GetEntityManager()->GetEntityById(playerNode.CellEntityID));
		cellMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("blue_square");
	}

	void Grid::DrawSeekerCell(Frosty::ECS::CTransform* transform)
	{
		if (!m_DrawGizmos) return;

		CellNode& enemyNode = WorldPointToNode(transform->Position);

		//if (enemyNode.Walkable)
		{
			auto& cellMat = m_World->GetComponent<Frosty::ECS::CMaterial>(m_World->GetEntityManager()->GetEntityById(enemyNode.CellEntityID));
			cellMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("purple_square");
			m_DynamicOccupiedNodes.emplace_back(&enemyNode);
		}
	}

	void Grid::DrawPathCells(const std::vector<CellNode*> path)
	{
		if (!m_DrawGizmos) return;

		for (size_t i = 0; i < path.size(); i++)
		{
			auto& cellMat = m_World->GetComponent<Frosty::ECS::CMaterial>(m_World->GetEntityManager()->GetEntityById(path[i]->CellEntityID));
			cellMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("brown_square");
			m_PathNodes.emplace_back(*path[i]);
		}
	}

	void Grid::CreateGrid()
	{
		m_CellNodes.reserve((size_t)(m_GridSize.x) * (size_t)(m_GridSize.y));
		glm::vec3 worldBottomLeft = m_GridWorldPosition - glm::vec3(1.0f, 0.0f, 0.0f) * m_GridWorldSize.x / 2.0f + glm::vec3(0.0f, 0.0f, 1.0f) * m_GridWorldSize.y / 2.0f;
		glm::vec3 worldPoint = glm::vec3(0.0f);
		bool walkable = true;

		for (int32_t y = 0; y < m_GridSize.y; y++)
		{
			for (int32_t x = 0; x < m_GridSize.x; x++)
			{
				worldPoint = worldBottomLeft + glm::vec3(1.0f, 0.0f, 0.0f) * (x * CELL_SIZE + CELL_SIZE * 0.5f) - glm::vec3(0.0f, 0.0f, 1.0f) * (y * CELL_SIZE + CELL_SIZE * 0.5f);
				walkable = !CheckCollision(worldPoint, CELL_SIZE * 0.25f);
				m_CellNodes.emplace_back(worldPoint, walkable, x, y);
			}
		}

		// "crossroad_chests_IsStatick_t_p_e_r_h";
		// "threeWayRoad_chests_IsStatick_t_p_e_r_h";
		// "turningRoad_chests_IsStatick_t_p_e_r_h";
		// "straightRoad_chests_IsStatick_t_p_e_r_h";
		// "deadend_chests_IsStatick_t_p_e_r_h";


		//SaveFile("straightRoad_chests_IsStatick_t_p_e_r_h");
		//LoadFile("TestGmap.Gmap");


		if (!m_DrawGizmos) return;

		for (size_t i = 0; i < m_CellNodes.size(); i++)
		{
			auto& cell = m_World->CreateEntity({ m_CellNodes[i].WorldPosition.x, 0.05, m_CellNodes[i].WorldPosition.z }, { 0.0f,0.0f,0.0f }, { CELL_SIZE, 1.0f, CELL_SIZE }, true);
			cell->ShowInEditor = false;
			m_World->AddComponent<Frosty::ECS::CMesh>(cell, Frosty::AssetManager::GetMesh("pPlane1"));
			auto& cellMat = m_World->AddComponent<Frosty::ECS::CMaterial>(cell, Frosty::AssetManager::GetShader("Texture2D"));
			cellMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D(m_CellNodes[i].Walkable ? "green_square" : "red_square");
			m_CellNodes[i].CellEntityID = cell->Id;
		}
	}

	void Grid::SaveFile(const std::string FileName)
	{
		//Worst Solution ever!
		std::ofstream OutFile;

		OutFile.open(FileName + ".gmap");
		if (OutFile.is_open())
		{
			OutFile << m_GridWorldPosition.x << "\n";
			OutFile << m_GridWorldPosition.y << "\n";
			OutFile << m_GridWorldPosition.z << "\n";
			OutFile << m_GridWorldSize.x << "\n";
			OutFile << m_GridWorldSize.y << "\n";
			OutFile << m_GridSize.x << "\n";
			OutFile << m_GridSize.y << "\n";



			uint32_t VecSize = m_CellNodes.size();
			OutFile << VecSize << "\n";


			for (int i = 0; i < VecSize; i++)
			{
				OutFile << m_CellNodes[i].WorldPosition[0] << "\n";
				OutFile << m_CellNodes[i].WorldPosition[1] << "\n";
				OutFile << m_CellNodes[i].WorldPosition[2] << "\n";

				OutFile << m_CellNodes[i].Walkable << "\n";
				OutFile << m_CellNodes[i].GridX << "\n";
				OutFile << m_CellNodes[i].GridY << "\n";

				OutFile << m_CellNodes[i].GCost << "\n";
				OutFile << m_CellNodes[i].HCost << "\n";

				OutFile << m_CellNodes[i].ParentGridX << "\n";
				OutFile << m_CellNodes[i].ParentGridY << "\n";

				OutFile << m_CellNodes[i].ExistsInOpenSet << "\n";

				OutFile << m_CellNodes[i].CellEntityID << "\n";
			}
		}
		else
		{
			FY_ASSERT(0, "Could not open Grid Map (Save Function)");
		}



		//FILE* File;

		//std::string Name = FileName + ".Gmap";

		//File = fopen(Name.c_str(), "w");

		//fwrite(&(m_GridWorldPosition[0]), sizeof(glm::vec3), 1, File);
		//fwrite(&(m_GridWorldSize[0]), sizeof(glm::vec2), 1, File);
		//fwrite(&(m_GridSize[0]), sizeof(glm::vec2), 1, File);

		//uint32_t VecSize = m_CellNodes.size();

		//fwrite(&VecSize, sizeof(uint32_t), 1, File);
		////int test = fwrite(&m_CellNodes[0], sizeof(struct CellNode), VecSize, File);
		////int test = 0;

		//for (int i = (VecSize-1); i >= 0; i--)
		//{
		//	if (1 != fwrite(&(m_CellNodes[i].WorldPosition[0]), sizeof(glm::vec3), 1, File)) __debugbreak();
		//	if (1 != fwrite(&(m_CellNodes[i].Walkable), sizeof(bool), 1, File)) __debugbreak();
		//	if (1 != fwrite(&(m_CellNodes[i].GridX), sizeof(int32_t), 1, File)) __debugbreak();
		//	if (1 != fwrite(&(m_CellNodes[i].GridY), sizeof(int32_t), 1, File)) __debugbreak();
		//	if (1 != fwrite(&(m_CellNodes[i].GCost), sizeof(int16_t), 1, File)) __debugbreak();
		//	if (1 != fwrite(&(m_CellNodes[i].HCost), sizeof(int16_t), 1, File)) __debugbreak();
		//	if (1 != fwrite(&(m_CellNodes[i].ParentGridX), sizeof(int32_t), 1, File)) __debugbreak();
		//	if (1 != fwrite(&(m_CellNodes[i].ParentGridY), sizeof(int32_t), 1, File)) __debugbreak();
		//	if (1 != fwrite(&(m_CellNodes[i].ExistsInOpenSet), sizeof(bool), 1, File)) __debugbreak();
		//	if (1 != fwrite(&(m_CellNodes[i].CellEntityID), sizeof(int32_t), 1, File)) __debugbreak();
		//}



		////if (test == m_CellNodes.size())
		////{
		////	int kl = 0;
		////}

		//fclose(File);
	}

	void Grid::LoadFile(const std::string FilePath)
	{


		std::ifstream InFile;

		InFile.open(FilePath);

		if (InFile.is_open())
		{

			//m_FilePath = FilePath;

			std::string temp;

			std::getline(InFile, temp);
			m_GridWorldPosition.x = std::stof(temp);
			std::getline(InFile, temp);
			m_GridWorldPosition.y = std::stof(temp);
			std::getline(InFile, temp);
			m_GridWorldPosition.z = std::stof(temp);

			std::getline(InFile, temp);
			m_GridWorldSize.x = std::stof(temp);
			std::getline(InFile, temp);
			m_GridWorldSize.y = std::stof(temp);

			std::getline(InFile, temp);
			m_GridSize.x = std::stof(temp);
			std::getline(InFile, temp);
			m_GridSize.y = std::stof(temp);


			std::getline(InFile, temp);
			uint32_t VecSize = std::stof(temp);
			//std::vector<CellNode> TestVec;
			//TestVec.resize(VecSize);
			m_CellNodes.resize(VecSize);


			for (int i = 0; i < VecSize; i++)
			{

				std::getline(InFile, temp);
				m_CellNodes[i].WorldPosition[0] = std::stof(temp);
				std::getline(InFile, temp);
				m_CellNodes[i].WorldPosition[1] = std::stof(temp);
				std::getline(InFile, temp);
				m_CellNodes[i].WorldPosition[2] = std::stof(temp);

				std::getline(InFile, temp);
				m_CellNodes[i].Walkable = std::stoi(temp);

				std::getline(InFile, temp);
				m_CellNodes[i].GridX = std::stoi(temp);
				std::getline(InFile, temp);
				m_CellNodes[i].GridY = std::stoi(temp);

				std::getline(InFile, temp);
				m_CellNodes[i].GCost = std::stoi(temp);
				std::getline(InFile, temp);
				m_CellNodes[i].HCost = std::stoi(temp);

				std::getline(InFile, temp);
				m_CellNodes[i].ParentGridX = std::stoi(temp);
				std::getline(InFile, temp);
				m_CellNodes[i].ParentGridY = std::stoi(temp);

				std::getline(InFile, temp);
				m_CellNodes[i].ExistsInOpenSet = std::stoi(temp);
				std::getline(InFile, temp);
				m_CellNodes[i].CellEntityID = std::stoi(temp);
			}






			//FILE* File;

			//File = fopen(FilePath.c_str(), "r");

			//if (File != nullptr)
			//{
			//	fread(&m_GridWorldPosition[0], sizeof(glm::vec3), 1, File);
			//	fread(&m_GridWorldSize[0], sizeof(glm::vec2), 1, File);
			//	fread(&m_GridSize[0], sizeof(glm::vec2), 1, File);



			//	uint32_t VecSize;
			//	fread(&VecSize, sizeof(uint32_t), 1, File);

			//	std::vector<CellNode> TestVec;
			//	TestVec.resize(VecSize);


			//	//	int test = fread(&TestVec[0], sizeof(struct CellNode), VecSize, File);

			//	for (int i = (VecSize-1); i >= 0; i--)
			//	{
			//		if (1 != fread(&(TestVec[i].WorldPosition[0]), sizeof(glm::vec3), 1, File)) __debugbreak();
			//		if(1 != fread(&(TestVec[i].Walkable), sizeof(bool), 1, File)) __debugbreak();
			//		if(1 != fread(&(TestVec[i].GridX), sizeof(int32_t), 1, File)) __debugbreak();
			//		if(1 != fread(&(TestVec[i].GridY), sizeof(int32_t), 1, File)) __debugbreak();
			//		if(1 != fread(&(TestVec[i].GCost), sizeof(int16_t), 1, File)) __debugbreak();
			//		if(1 != fread(&(TestVec[i].HCost), sizeof(int16_t), 1, File)) __debugbreak();
			//		if(1 != fread(&(TestVec[i].ParentGridX), sizeof(int32_t), 1, File)) __debugbreak();
			//		if(1 != fread(&(TestVec[i].ParentGridY), sizeof(int32_t), 1, File)) __debugbreak();
			//		if(1 != fread(&(TestVec[i].ExistsInOpenSet), sizeof(bool), 1, File)) __debugbreak();
			//		if(1 != fread(&(TestVec[i].CellEntityID), sizeof(int32_t), 1, File)) __debugbreak();
			//	}


			//TestCase
			//for (int i = 0; i < m_CellNodes.size(); i++)
			//{
			//	if (m_CellNodes.at(i) != TestVec.at(i))
			//	{
			//		int nein = 9;
			//	}
			//}

		}
		else
		{
			FY_ASSERT(0, "Could not open Grid Map (Load Function)");
		}




		//}
		//else
		//{
		//	FY_ASSERT(0, "Could not load Grid Map");
		//}

		//fclose(File);
	}

	bool Grid::CheckCollision(const glm::vec3& worldPoint, float radius) const
	{
		// Collision
		auto& physicsCompData = m_World->GetComponentManager<Frosty::ECS::CPhysics>()->GetAll();
		size_t totalData = m_World->GetComponentManager<Frosty::ECS::CPhysics>()->GetTotal();

		// Check which entities should be included as obstacles
		for (size_t i = 1; i < totalData; i++)
		{
			auto& tempTransform = m_World->GetComponent<Frosty::ECS::CTransform>(physicsCompData[i].EntityPtr);
			//if (tempTransform.IsStatic)
			if (!m_World->HasComponent<Frosty::ECS::CEnemy>(tempTransform.EntityPtr) && !m_World->HasComponent<Frosty::ECS::CPlayer>(tempTransform.EntityPtr))
			{
				// This means that the entity is static and has a CPhysics component
				auto& tempPhysics = m_World->GetComponent<Frosty::ECS::CPhysics>(tempTransform.EntityPtr);
				glm::vec4 collisionBox = glm::vec4(
					tempTransform.Position.x - tempTransform.Scale.x * tempPhysics.BoundingBox->halfSize[0] - radius,
					tempTransform.Position.z + tempTransform.Scale.z * tempPhysics.BoundingBox->halfSize[2] + radius,
					tempTransform.Position.x + tempTransform.Scale.x * tempPhysics.BoundingBox->halfSize[0] + radius,
					tempTransform.Position.z - tempTransform.Scale.z * tempPhysics.BoundingBox->halfSize[2] - radius
				);

				if (worldPoint.x > collisionBox.x && worldPoint.x < collisionBox.z &&
					worldPoint.z < collisionBox.y && worldPoint.z > collisionBox.w)
				{
					return true;
				}
			}
		}

		return false;
	}
}