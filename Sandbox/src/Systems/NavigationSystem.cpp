#include <mcspch.hpp>
#include "NavigationSystem.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{
	const std::string NavigationSystem::NAME = "Navigation";

	void NavigationSystem::Init()
	{
		m_World = Frosty::Application::Get().GetWorld().get();
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CPhysics>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CEnemy>(), true);
	}

	void NavigationSystem::OnUpdate()
	{
		m_Grid->Reset();
		for (size_t i = 1; i < p_Total; i++)
		{
			m_Grid->SetNodeUnwalkable(m_Transform[i]->Position);
		}

		for (size_t i = 1; i < p_Total; i++)
		{
			m_Grid->DrawSeekerCell(m_Transform[i]);

			// Check current state
			switch (m_Enemy[i]->CurrentState)
			{
			case Frosty::ECS::CEnemy::State::Idle:
				m_Physics[i]->SpeedMultiplier = 1.0f;
				m_Physics[i]->Direction = glm::vec3(0.0f);
				break;
			case Frosty::ECS::CEnemy::State::Attack:
				HandleDistance(i);
				break;
			case Frosty::ECS::CEnemy::State::Escape:
				HandleEscape(i);
				break;
			case Frosty::ECS::CEnemy::State::Chase:
				HandlePathfinding(i);
				break;
			case Frosty::ECS::CEnemy::State::Reset:
				HandleReset(i);
				break;
			case Frosty::ECS::CEnemy::State::Dead:
				HandleDeath(i);
				break;
			default:
				break;
			}
		}
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

			m_Enemy[p_Total]->SpawnPosition = m_Transform[p_Total]->Position;

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
		m_Grid.reset(FY_NEW Grid());
		Frosty::Time::StartTimer("Grid::Init()");
		m_Grid->Init(planeTransform);
		Frosty::Time::EndTimer("Grid::Init()");

		m_Pathfinding.reset(FY_NEW Pathfinding());
		m_Pathfinding->Init(m_Grid.get());
	}

	void NavigationSystem::OnInitiateGridMap(Frosty::InitiateGridEvent& e)
	{
		m_Grid.reset(FY_NEW Grid());
		Frosty::Time::StartTimer("Grid::Init()");
		m_Grid->Init(*e.GetTransform());
		Frosty::Time::EndTimer("Grid::Init()");

		m_Pathfinding.reset(FY_NEW Pathfinding());
		m_Pathfinding->Init(m_Grid.get());
	}

	void NavigationSystem::LookAtPoint(const glm::vec3& point, size_t index)
	{
		// Target vector
		glm::vec3 pointVector = glm::normalize(point - m_Transform[index]->Position);

		// Origin vector
		glm::mat4 mat = glm::mat4(1.0f);
		mat = glm::rotate(mat, glm::radians(m_Transform[index]->Rotation.x), { 1.0f, 0.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(m_Transform[index]->Rotation.y), { 0.0f, 1.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(m_Transform[index]->Rotation.z), { 0.0f, 0.0f, 1.0f });
		glm::vec3 dir = glm::normalize(mat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0));

		// Calculate angle (dot)
		double angle = glm::dot(pointVector, dir);

		// Convert dot angle to degrees
		float totalDegrees = (float)glm::degrees(glm::acos(angle));
		float rotationDegrees = 200.0f * Frosty::Time::DeltaTime();
		if (rotationDegrees > totalDegrees)
		{
			rotationDegrees = totalDegrees;
		}

		// Check to see minus/plus
		glm::vec3 newRotation = m_Transform[index]->Rotation;
		newRotation.y += rotationDegrees;

		// Get the new direction based on rotation
		mat = glm::mat4(1.0f);
		mat = glm::rotate(mat, glm::radians(newRotation.x), { 1.0f, 0.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(newRotation.y), { 0.0f, 1.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(newRotation.z), { 0.0f, 0.0f, 1.0f });
		glm::vec3 newDir = glm::normalize(mat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0));

		// Check to see if we rotated towards or away from target
		double newAngle = glm::dot(pointVector, newDir);
		if (newAngle < angle)
		{
			// We rotated wrong way
			newRotation = m_Transform[index]->Rotation;
			newRotation.y -= rotationDegrees;
		}

		// Set the rotation
		m_Transform[index]->Rotation = newRotation;
	}
	
	void NavigationSystem::HandlePathfinding(size_t index)
	{
		// Check if enemy is boss and check active ability
		if (m_World->HasComponent<Frosty::ECS::CBoss>(m_Transform[index]->EntityPtr))
		{
			auto& bossComp = m_World->GetComponent<Frosty::ECS::CBoss>(m_Transform[index]->EntityPtr);
			if (bossComp.ActiveAbility != Frosty::ECS::CBoss::AbilityState::None) return;
		}
		
		// Find target cell and set velocity
		glm::vec3 cellTarget = m_Pathfinding->FindPath(m_Transform[index]->Position, m_Enemy[index]->Target->Position);
		m_Enemy[index]->CellTarget = cellTarget;
		m_Physics[index]->Direction = glm::normalize(m_Enemy[index]->CellTarget - glm::vec3(m_Transform[index]->Position.x, 0.0f, m_Transform[index]->Position.z));

		// Rotate towards target (cell)
		LookAtPoint(cellTarget, index);


		//// Check if enemy has a target
		//if (m_Enemy[index]->Target != nullptr && m_Enemy[index]->Weapon != nullptr)
		//{
		//	// Check if it is outside attack range and inside sight range
		//	if (glm::distance(glm::vec2(m_Transform[index]->Position.x, m_Transform[index]->Position.z), glm::vec2(m_Enemy[index]->Target->Position.x, m_Enemy[index]->Target->Position.z)) > m_Enemy[index]->Weapon->MaxAttackRange&&
		//		glm::distance(glm::vec2(m_Transform[index]->Position.x, m_Transform[index]->Position.z), glm::vec2(m_Enemy[index]->Target->Position.x, m_Enemy[index]->Target->Position.z)) <= m_Enemy[index]->SightRange)
		//	{
		//		// Find target cell and set velocity
		//		glm::vec3 cellTarget = m_Pathfinding->FindPath(m_Transform[index]->Position, m_Enemy[index]->Target->Position);
		//		m_Enemy[index]->CellTarget = cellTarget;
		//		m_Physics[index]->Velocity = glm::normalize(m_Enemy[index]->CellTarget - glm::vec3(m_Transform[index]->Position.x, 0.0f, m_Transform[index]->Position.z)) * m_Physics[index]->Speed;
		//
		//		// Rotate towards target (cell)
		//		LookAtPoint(cellTarget, index);
		//	}
		//	else
		//	{
		//		if (glm::distance(glm::vec2(m_Transform[index]->Position.x, m_Transform[index]->Position.z), glm::vec2(m_Enemy[index]->Target->Position.x, m_Enemy[index]->Target->Position.z)) > m_Enemy[index]->Weapon->MinAttackRange)
		//		{
		//			// Make the enemy stop by reseting velocity
		//			m_Physics[index]->Velocity = glm::vec3(0.0f);
		//		}
		//	}
		//}
	}

	void NavigationSystem::HandleDistance(size_t index)
	{
		// Check if enemy is boss
		if (m_World->HasComponent<Frosty::ECS::CBoss>(m_Transform[index]->EntityPtr)) return;
		//if (m_Enemy[index]->Weapon->AnimPlaying) return;

		if (glm::distance(m_Transform[index]->Position, m_Enemy[index]->Target->Position) >= m_Enemy[index]->Weapon->MinAttackRange)
		{
			m_Physics[index]->Direction = glm::vec3(0.0f);
		}
		else if (glm::distance(m_Transform[index]->Position, m_Enemy[index]->Target->Position) < m_Enemy[index]->Weapon->MinAttackRange)
		{
			m_Physics[index]->Direction = glm::normalize(glm::vec3(m_Transform[index]->Position.x, 0.0f, m_Transform[index]->Position.z) - m_Enemy[index]->Target->Position);
		}
	}
	
	void NavigationSystem::HandleEscape(size_t index)
	{
		m_Physics[index]->SpeedMultiplier = 1.5f;
		glm::vec3 escapeVector = glm::vec3(m_Transform[index]->Position.x, 0.0f, m_Transform[index]->Position.z) - m_Enemy[index]->Target->Position;
		LookAtPoint(escapeVector * 5.0f, index);
		m_Physics[index]->Direction = glm::normalize(escapeVector);
	}	

	void NavigationSystem::HandleReset(size_t index)
	{
		// Find target cell and set velocity
		m_Physics[index]->SpeedMultiplier = 1.5f;
		glm::vec3 cellTarget = m_Pathfinding->FindPath(m_Transform[index]->Position, m_Enemy[index]->SpawnPosition);
		m_Enemy[index]->CellTarget = cellTarget;
		m_Physics[index]->Direction = glm::normalize(m_Enemy[index]->CellTarget - glm::vec3(m_Transform[index]->Position.x, 0.0f, m_Transform[index]->Position.z));

		// Rotate towards target (cell)
		LookAtPoint(cellTarget, index);
	}
	
	void NavigationSystem::HandleDeath(size_t index)
	{
		m_Physics[index]->SpeedMultiplier = 0.0f;
		/*m_Physics[index]->Direction = glm::vec3(0.0f);*/
	}
}
