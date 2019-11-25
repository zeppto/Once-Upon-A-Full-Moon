#include <mcspch.hpp>
#include "NavigationSystem.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{
	const std::string NavigationSystem::NAME = "Navigation";

	void NavigationSystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CPhysics>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CEnemy>(), true);
	}

	void NavigationSystem::OnUpdate()
	{
		bool printTime = false;
		//if (Frosty::Time::GetFrameCount() % 60 == 0) printTime = true;

		if (printTime) Frosty::Time::StartTimer("Grid::Reset() - ALL");
		m_Grid->Reset();
		if (printTime) { Frosty::Time::EndTimer("Grid::Reset() - ALL"); }

		//if (p_Total >= 1)
		//{
		//	m_Grid->DrawTargetCell(m_Enemy[1]->Target);
		//}

		if (printTime) Frosty::Time::StartTimer("Grid::SetNodeUnwalkable() - ALL");
		for (size_t i = 1; i < p_Total; i++)
		{
			m_Grid->SetNodeUnwalkable(m_Transform[i]->Position);
		}
		if (printTime) { Frosty::Time::EndTimer("Grid::SetNodeUnwalkable() - ALL"); }
		
		if (printTime) Frosty::Time::StartTimer("Find path and draw enemy path - ALL");
		for (size_t i = 1; i < p_Total; i++)
		{
			m_Grid->DrawSeekerCell(m_Transform[i]);

			if (m_Enemy[i]->Target != nullptr)
			{
				if (glm::distance(glm::vec2(m_Transform[i]->Position.x, m_Transform[i]->Position.z), glm::vec2(m_Enemy[i]->Target->Position.x, m_Enemy[i]->Target->Position.z)) > m_Enemy[i]->AttackRange &&
					glm::distance(glm::vec2(m_Transform[i]->Position.x, m_Transform[i]->Position.z), glm::vec2(m_Enemy[i]->Target->Position.x, m_Enemy[i]->Target->Position.z)) <= m_Enemy[i]->SightRange)
				{
					glm::vec3 cellTarget = m_Pathfinding->FindPath(m_Transform[i]->Position, m_Enemy[i]->Target->Position);
					m_Enemy[i]->CellTarget = cellTarget;
					m_Physics[i]->Velocity = glm::normalize(m_Enemy[i]->CellTarget - glm::vec3(m_Transform[i]->Position.x, 0.0f, m_Transform[i]->Position.z)) * m_Physics[i]->Speed;
				}
				else
				{
					m_Physics[i]->Velocity = glm::vec3(0.0f);
				}
			}
		}
		if (printTime) { Frosty::Time::EndTimer("Find path and draw enemy path - ALL"); std::cout << "\n"; printTime = false; }
		

		/*
		// Reset all cells and give occupied cells -1 weight
		m_GridMap->ResetCellWeights();
		for (size_t i = 1; i < p_Total; i++)
		{
			m_GridMap->UpdateOccupiedCells(m_Transform[i]->EntityPtr->Id, m_Transform[i], &*m_Physics[i]->BoundingBox);
		}
		
		// Calculate route
		for (size_t i = 1; i < p_Total; i++)
		{
			// Reset
			//m_Physics[i]->Velocity = glm::vec3(0.0f);

			// Check sight range
			if (glm::distance(glm::vec2(m_Transform[i]->Position.x, m_Transform[i]->Position.z), glm::vec2(m_Enemy[i]->Target->Position.x, m_Enemy[i]->Target->Position.z)) <= m_Enemy[i]->SightRange)
			{
				// Check distance to cell target or if enemy has a cell target
				float distanceFromCell = glm::distance(glm::vec2(m_Transform[i]->Position.x, m_Transform[i]->Position.z), glm::vec2(m_Enemy[i]->CellTarget.x, m_Enemy[i]->CellTarget.z));
				FY_INFO("{0}", distanceFromCell);
				if (distanceFromCell <= 0.5f || m_Enemy[i]->CellTarget == glm::vec3(0.0f))
				{
					m_Enemy[i]->CellTarget = m_GridMap->GetDestination(m_Transform[i]->EntityPtr->Id, m_Transform[i]->Position, m_Enemy[i]->Target->Position);
					FY_INFO("({0}, {1}, {2})", m_Enemy[i]->CellTarget.x, m_Enemy[i]->CellTarget.y, m_Enemy[i]->CellTarget.z);
					m_Physics[i]->Velocity = glm::normalize(m_Enemy[i]->CellTarget - glm::vec3(m_Transform[i]->Position.x, 0.0f, m_Transform[i]->Position.z)) * m_Physics[i]->Speed;

					// Check attack range
					if (glm::distance(glm::vec2(m_Transform[i]->Position.x, m_Transform[i]->Position.z), glm::vec2(m_Enemy[i]->Target->Position.x, m_Enemy[i]->Target->Position.z)) <= m_Enemy[i]->AttackRange)
					{
						m_Physics[i]->Velocity = glm::vec3(0.0f);
						m_Enemy[i]->CellTarget = glm::vec3(0.0f);
					}
				}
			}
		}*/
	}

	void NavigationSystem::OnEvent(Frosty::BaseEvent& e)
	{
		switch (e.GetEventType())
		{
		case Frosty::EventType::InitiateGridMap:
			OnInitiateGridMap(static_cast<Frosty::InitiateGridEvent&>(e));
			break;
		default:
			break;
		}
	}

	void NavigationSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Physics[p_Total] = &world->GetComponent<Frosty::ECS::CPhysics>(entity);
			m_Enemy[p_Total] = &world->GetComponent<Frosty::ECS::CEnemy>(entity);

			p_Total++;
		}
	}

	void NavigationSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_Physics[p_Total] = nullptr;
			m_Enemy[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void NavigationSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CPhysics* physicsPtr = world->GetComponentAddress<Frosty::ECS::CPhysics>(entity);
			Frosty::ECS::CEnemy* enemyPtr = world->GetComponentAddress<Frosty::ECS::CEnemy>(entity);

			m_Transform[it->second] = transformPtr;
			m_Physics[it->second] = physicsPtr;
			m_Enemy[it->second] = enemyPtr;
		}
	}

	std::string NavigationSystem::GetInfo() const
	{
		std::stringstream retInfo;
		retInfo << "\t-----------" << NAME << " System Info-----------\n";
		retInfo << "\t\t---------Entity Map---------\n";
		retInfo << "\t\tEntity Id\tEntity Address\t\tEntity Refs\tArray Index\n";
		for (auto& em : p_EntityMap)
		{
			retInfo << "\t\t" << em.first->Id << "\t\t" << em.first << "\t\t" << em.first.use_count() << "\t" << em.second << "\n";
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t\t------Component Array(s)------\n";
		retInfo << "\n\t\tIndex\tComponent Address\tEntity Id\tEntity Address\t\tEntity Refs\n";
		for (size_t i = 1; i < p_Total; i++)
		{
			retInfo << "\t\t" << i << "\t" << m_Transform[i] << "\t" << m_Transform[i]->EntityPtr->Id << "\t\t" << m_Transform[i]->EntityPtr << "\t\t" << m_Transform[i]->EntityPtr.use_count() << "\n";
			retInfo << "\t\t" << i << "\t" << m_Physics[i] << "\t" << m_Physics[i]->EntityPtr->Id << "\t\t" << m_Physics[i]->EntityPtr << "\t\t" << m_Physics[i]->EntityPtr.use_count() << "\n";
			retInfo << "\t\t" << i << "\t" << m_Enemy[i] << "\t" << m_Enemy[i]->EntityPtr->Id << "\t\t" << m_Enemy[i]->EntityPtr << "\t\t" << m_Enemy[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}

	void NavigationSystem::InitiateGridMap(const Frosty::ECS::CTransform& planeTransform)
	{
		m_Grid.reset(FY_NEW Frosty::Grid());
		Frosty::Time::StartTimer("Grid::Init()");
		m_Grid->Init(planeTransform);
		Frosty::Time::EndTimer("Grid::Init()");

		m_Pathfinding.reset(FY_NEW Pathfinding());
		m_Pathfinding->Init(m_Grid.get());
	}

	void NavigationSystem::OnInitiateGridMap(Frosty::InitiateGridEvent& e)
	{
		m_Grid.reset(FY_NEW Frosty::Grid());
		Frosty::Time::StartTimer("Grid::Init()");
		m_Grid->Init(*e.GetTransform());
		Frosty::Time::EndTimer("Grid::Init()");

		m_Pathfinding.reset(FY_NEW Pathfinding());
		m_Pathfinding->Init(m_Grid.get());
	}
}
