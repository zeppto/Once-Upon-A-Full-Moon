#include <mcspch.hpp>
#include "InventorySystem.hpp"

namespace MCS
{

	void InventorySystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CInventory>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CMotion>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);

		// EventBus::GetEventBus()->Subscribe<Application, BaseEvent>(this, &Application::OnEvent);
		Frosty::EventBus::GetEventBus()->Subscribe<InventorySystem, Frosty::BaseEvent>(this, &InventorySystem::OnEvent);

	}

	void InventorySystem::OnUpdate()
	{
		// Only check the timer for temp speed, reset speed after timer runs out
		for (size_t i = 1; i < p_Total; i++)
		{
			if ((float(std::clock()) - m_Inventory[i]->SpeedTimer) * 0.001 >= m_Inventory[i]->SpeedCooldown)
			{
				m_Motion[i]->SpeedMultiplier = 1.f;
			}
		}
	}

	void InventorySystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Inventory[p_Total] = &world->GetComponent<Frosty::ECS::CInventory>(entity);
			m_Motion[p_Total] = &world->GetComponent<Frosty::ECS::CMotion>(entity);
			m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);

			p_Total++;
		}
	}

	void InventorySystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];


		if (tempIndex > 0)
		{
			p_Total--;
			m_Inventory[p_Total] = nullptr;
			m_Motion[p_Total] = nullptr;
			m_Health[p_Total] = nullptr;

			if (p_Total > 1)
			{
				//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

				if (p_Total > tempIndex)
				{
					std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Inventory[p_EntityMap[entity]]->EntityPtr;
					p_EntityMap[entityToUpdate] = tempIndex;
				}
			}

			p_EntityMap.erase(entity);
		}
	}

	void InventorySystem::OnEvent(Frosty::BaseEvent& e)
	{
		switch (e.GetEventType())
		{
		case Frosty::EventType::KeyPressed:
			OnKeyPressedEvent(static_cast<Frosty::KeyPressedEvent&>(e));
			break;
		default:
			break;
		}
	}

	void InventorySystem::OnKeyPressedEvent(Frosty::KeyPressedEvent& e)
	{
		if (e.GetKeyCode() == FY_KEY_1)	// HEALING POTION
		{
			for (size_t i = 1; i < p_Total; i++)
			{
				// If consumer has healing potion AND comsumer has not full health AND healing timer is bigger than cooldown--> drink healing potion
				if ((m_Inventory[i]->CurrentHealingPotions > 0) && (m_Health[i]->CurrentHealth != m_Health[i]->MaxHealth) && ((float(std::clock()) - m_Inventory[i]->HealingTimer) * 0.001 >= m_Inventory[i]->HealingCooldown))
				{
					if (m_Inventory[i]->Heal <= (m_Health[i]->MaxHealth - m_Health[i]->CurrentHealth))
					{
						m_Health[i]->CurrentHealth += m_Inventory[i]->Heal;
					}
					else
					{
						m_Health[i]->CurrentHealth += m_Health[i]->MaxHealth - m_Health[i]->CurrentHealth;
					}

					m_Inventory[i]->CurrentHealingPotions--;
					m_Inventory[i]->HealingTimer = float(std::clock());
				}
			}
		}
		else if (e.GetKeyCode() == FY_KEY_2)	// INCREASE HEALTH POTION
		{
			for (size_t i = 1; i < p_Total; i++)
			{
				// If consumer has increase HP potion AND comsumer can increse health AND increase HP timer is bigger than cooldown--> drink increase HP potion
				if ((m_Inventory[i]->CurrentIncreaseHPPotions > 0) && (m_Health[i]->MaxHealth != m_Health[i]->MaxTotalHealth) && ((float(std::clock()) - m_Inventory[i]->IncreaseHPTimer) * 0.001 >= m_Inventory[i]->IncreaseHPCooldown))
				{
					if (m_Inventory[i]->IncreaseHP <= (m_Health[i]->MaxTotalHealth - m_Health[i]->MaxHealth))
					{
						m_Health[i]->MaxHealth += m_Inventory[i]->IncreaseHP;
					}
					else
					{
						m_Health[i]->MaxHealth += m_Health[i]->MaxTotalHealth - m_Health[i]->MaxHealth;
					}

					m_Inventory[i]->CurrentIncreaseHPPotions--;
					m_Inventory[i]->IncreaseHPTimer = float(std::clock());
				}
			}
		}
		else if (e.GetKeyCode() == FY_KEY_3)	// SPEED BOOSTER POTION
		{
			for (size_t i = 1; i < p_Total; i++)
			{
				// If consumer has speed potion AND comsumer has not full speed capacity AND speed timer is bigger than cooldown--> drink speed boost potion
				if ((m_Inventory[i]->CurrentSpeedPotions > 0) && (m_Motion[i]->Speed != m_Motion[i]->maxSpeed) && ((float(std::clock()) - m_Inventory[i]->SpeedTimer) * 0.001 >= m_Inventory[i]->SpeedCooldown))
				{
					if (((m_Inventory[i]->IncreaseSpeedTemporary + m_Motion[i]->SpeedMultiplier) * m_Motion[i]->Speed) <= m_Motion[i]->maxSpeed)
					{
						m_Motion[i]->SpeedMultiplier += m_Inventory[i]->IncreaseSpeedTemporary;
					}
					else
					{
						m_Motion[i]->SpeedMultiplier = m_Motion[i]->maxSpeed / m_Motion[i]->Speed;
					}

					m_Inventory[i]->CurrentSpeedPotions--;
					m_Inventory[i]->SpeedTimer = float(std::clock());
				}
			}
		}
		else if (e.GetKeyCode() == FY_KEY_4)	// SPEED BOOTS
		{
			for (size_t i = 1; i < p_Total; i++)
			{
				// If consumer has speed potion AND comsumer has not full speed capacity
				if ((m_Inventory[i]->CurrentSpeedBoots < m_Inventory[i]->MaxSpeedBoots) && (m_Motion[i]->Speed != m_Motion[i]->maxSpeed))
				{
					if (m_Inventory[i]->IncreaseSpeed <= (m_Motion[i]->maxSpeed - m_Motion[i]->Speed))
					{
						m_Motion[i]->Speed += m_Inventory[i]->IncreaseSpeed;
					}
					else
					{
						m_Motion[i]->Speed += m_Motion[i]->maxSpeed - m_Motion[i]->Speed;
					}

					m_Inventory[i]->CurrentSpeedBoots++;
				}
			}
		}
	}
}