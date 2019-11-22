#include <mcspch.hpp>
#include "CombatSystem.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{
	const std::string CombatSystem::NAME = "Combat";

	void CombatSystem::Init()
	{
		m_World = Frosty::Application::Get().GetWorld().get();

		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
	}

	void CombatSystem::OnUpdate()
	{
	}

	void CombatSystem::OnEvent(Frosty::BaseEvent& e)
	{
		switch (e.GetEventType())
		{
		case Frosty::EventType::Collision:
			OnCollisionEvent(static_cast<Frosty::CollisionEvent&>(e));
			break;
		default:
			break;
		}
	}

	void CombatSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);

			p_Total++;
		}
	}

	void CombatSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);
		auto& world = Frosty::Application::Get().GetWorld();

		if (it != p_EntityMap.end())
		{
			p_Total--;
			
			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_Health[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void CombatSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CHealth* healthPtr = world->GetComponentAddress<Frosty::ECS::CHealth>(entity);

			m_Transform[it->second] = transformPtr;
			m_Health[it->second] = healthPtr;
		}
	}

	std::string CombatSystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_Health[i] << "\t" << m_Health[i]->EntityPtr->Id << "\t\t" << m_Health[i]->EntityPtr << "\t\t" << m_Health[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}

	void CombatSystem::OnCollisionEvent(Frosty::CollisionEvent& e)
	{
		// Entity A is attack and Entity B is either player, enemy or chest
		// Entity A doesn't exist in this system but Entity B must
		auto& entityA = e.GetEntityA();						// Attack entity
		auto& it = p_EntityMap.find(e.GetEntityB());		// Other entity (Player/Enemy)

		if (it == p_EntityMap.end()) return;

		if (m_World->HasComponent<Frosty::ECS::CPlayer>(it->first))
		{
			auto& attackComp = m_World->GetComponent<Frosty::ECS::CAttack>(entityA);
			if (Frosty::utils::BinarySearch(attackComp.AttackedEntities, it->first->Id) == -1)
			{
				attackComp.AttackedEntities.emplace_back(it->first->Id);
				m_Health[it->second]->CurrentHealth -= attackComp.Damage;
			}
		}
		else if (m_World->HasComponent<Frosty::ECS::CEnemy>(it->first))
		{
			auto& enemyComp = m_World->GetComponent<Frosty::ECS::CEnemy>(it->first);
			if (enemyComp.CurrentState != Frosty::ECS::CEnemy::State::Reset)
			{
				auto& attackComp = m_World->GetComponent<Frosty::ECS::CAttack>(entityA);
				if (Frosty::utils::BinarySearch(attackComp.AttackedEntities, it->first->Id) == -1)
				{
					attackComp.AttackedEntities.emplace_back(it->first->Id);
					m_Health[it->second]->CurrentHealth -= attackComp.Damage;
				}
			}
		}

		// Check if attack should be destroyed on hit
		if (m_World->GetComponent<Frosty::ECS::CAttack>(entityA).Destroyable && !m_World->HasComponent<Frosty::ECS::CDestroy>(entityA))
		{
			m_World->AddComponent<Frosty::ECS::CDestroy>(entityA);
		}

	}
}