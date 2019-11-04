#include <mcspch.hpp>
#include "ConsumablesSystem.hpp"

void ConsumablesSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CConsumables>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CMotion>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
}

void ConsumablesSystem::OnInput()
{
	if (Frosty::InputManager::IsKeyPressed(FY_KEY_E))
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			// If consumer has healing potion AND comsumer has not full health AND healing timer is bigger than cooldown--> drink healing potion
			if ((m_Consumables[i]->CurrentNrOfHealingPotions > 0) && (m_Health[i]->CurrentHealth != m_Health[i]->MaxHealth) && ((float(std::clock()) - m_Consumables[i]->HealingTimer) * 0.001 >= m_Consumables[i]->HealingCooldown))
			{
				if (m_Consumables[i]->Heal <= (m_Health[i]->MaxHealth - m_Health[i]->CurrentHealth))
				{
					m_Health[i]->CurrentHealth += m_Consumables[i]->Heal;
				}
				else
				{
					m_Health[i]->CurrentHealth += m_Health[i]->MaxHealth - m_Health[i]->CurrentHealth;
				}

				m_Consumables[i]->CurrentNrOfHealingPotions--;
				m_Consumables[i]->HealingTimer = float(std::clock());
			}
		}
	}
}

void ConsumablesSystem::OnUpdate()
{

}

void ConsumablesSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
	{
		p_EntityMap.emplace(entity, p_Total);

		auto& world = Frosty::Application::Get().GetWorld();
		m_Consumables[p_Total] = &world->GetComponent<Frosty::ECS::CConsumables>(entity);
		m_Motion[p_Total] = &world->GetComponent<Frosty::ECS::CMotion>(entity);
		m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);

		p_Total++;
	}
}

void ConsumablesSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];


	if (tempIndex > 0)
	{
		p_Total--;
		m_Consumables[p_Total] = nullptr;
		m_Motion[p_Total] = nullptr;
		m_Health[p_Total] = nullptr;

		if (p_Total > 1)
		{
			//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

			if (p_Total > tempIndex)
			{
				std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Consumables[p_EntityMap[entity]]->EntityPtr;
				p_EntityMap[entityToUpdate] = tempIndex;
			}
		}

		p_EntityMap.erase(entity);
	}
}