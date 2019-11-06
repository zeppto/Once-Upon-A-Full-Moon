#include <mcspch.hpp>
#include "CombatSystem.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{
	void CombatSystem::Init()
	{
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
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

		if (tempIndex > 0)
		{
			p_Total--;
			m_Transform[p_Total] = nullptr;
			m_Health[p_Total] = nullptr;

			//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

			if (p_Total > tempIndex)
			{
				std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
				p_EntityMap[entityToUpdate] = tempIndex;
			}

			p_EntityMap.erase(entity);
		}
	}
	
	void CombatSystem::OnCollisionEvent(Frosty::CollisionEvent& e)
	{
		size_t indexA = p_EntityMap[e.GetEntityA()];
		size_t indexB = p_EntityMap[e.GetEntityB()];
		auto& world = Frosty::Application::Get().GetWorld();

		//world->RemoveEntity(e.GetEntityA());
		if (indexA > 0)
		{
			if (world->HasComponent<Frosty::ECS::CArrow>(e.GetEntityB()) || world->HasComponent<Frosty::ECS::CSword>(e.GetEntityB()))
			{
				if (!world->HasComponent<Frosty::ECS::CDestroy>(e.GetEntityA()))
				{
					world->AddComponent<Frosty::ECS::CDestroy>(e.GetEntityA());
				}
			}
		}
		if (indexB > 0)
		{
			if (world->HasComponent<Frosty::ECS::CArrow>(e.GetEntityA()) || world->HasComponent<Frosty::ECS::CSword>(e.GetEntityA()))
			{
				if (!world->HasComponent<Frosty::ECS::CDestroy>(e.GetEntityB()))
				{
					world->AddComponent<Frosty::ECS::CDestroy>(e.GetEntityB());
				}
			}
		}
	}
}