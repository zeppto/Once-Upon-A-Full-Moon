#include <mcspch.hpp>
#include "PlayerControllerSystem.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{
	void PlayerControllerSystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CPlayer>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CDash>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CBasicAttack>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CPhysics>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CInventory>(), true);
	}

	void PlayerControllerSystem::OnInput()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			// Reset (Should this be here? Maybe release instead)

			// Maybe move these functions somewhere else so they can be used by other systems as well.
			// Get the point on our terrain
			glm::vec3 point3D = ScreenToTerrainPoint();

			// Look at point
			LookAtPoint(point3D, i);

			// Input
			if (!m_Dash[i]->Active)
			{
				HandleMovement(i);
				HandleAttack(point3D, i);
				HandleInventory(i);
			}

		}
	}

	void PlayerControllerSystem::OnUpdate()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Dash[i]->CurrentCooldown > 0.0) m_Dash[i]->CurrentCooldown -= Frosty::Time::DeltaTime();

			if (m_Dash[i]->Active)
			{
				m_Dash[i]->DistanceDashed += glm::length(m_Physics[i]->Velocity * Frosty::Time::DeltaTime());
				if (m_Dash[i]->DistanceDashed >= m_Dash[i]->DISTANCE / 1000.0f)
				{
					m_Dash[i]->Active = false;
					m_Dash[i]->DistanceDashed = 0.0f;
				}
			}
		}
	}

	void PlayerControllerSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Player[p_Total] = &world->GetComponent<Frosty::ECS::CPlayer>(entity);
			m_Dash[p_Total] = &world->GetComponent<Frosty::ECS::CDash>(entity);
			m_BasicAttack[p_Total] = &world->GetComponent<Frosty::ECS::CBasicAttack>(entity);
			m_Physics[p_Total] = &world->GetComponent<Frosty::ECS::CPhysics>(entity);
			m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);
			m_Inventory[p_Total] = &world->GetComponent<Frosty::ECS::CInventory>(entity);

			p_Total++;
		}
	}

	void PlayerControllerSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

		if (tempIndex > 0)
		{
			p_Total--;
			m_Transform[p_Total] = nullptr;
			m_Player[p_Total] = nullptr;
			m_Dash[p_Total] = nullptr;
			m_BasicAttack[p_Total] = nullptr;
			m_Physics[p_Total] = nullptr;
			m_Health[p_Total] = nullptr;
			m_Inventory[p_Total] = nullptr;


			if (p_Total > 1)
			{
				//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

				if (p_Total > tempIndex)
				{
					std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
					p_EntityMap[entityToUpdate] = tempIndex;
				}
			}

			p_EntityMap.erase(entity);
		}
	}

	glm::vec3 PlayerControllerSystem::ScreenToTerrainPoint()
	{
		glm::vec4 viewport = Frosty::Application::Get().GetWindow().GetViewport();
		auto& gameCameraEntity = Frosty::Application::Get().GetWorld()->GetSceneCamera();
		auto& gameCameraTransformComp = Frosty::Application::Get().GetWorld()->GetComponent<Frosty::ECS::CTransform>(gameCameraEntity);
		auto& gameCameraCamerComp = Frosty::Application::Get().GetWorld()->GetComponent<Frosty::ECS::CCamera>(gameCameraEntity);
		glm::vec2 mousePos = glm::vec2(Frosty::InputManager::GetMouseX(), Frosty::InputManager::GetMouseY());


		// Convert from viewport to NDC
		glm::vec2 NDC = glm::vec2(
			(2.0f * (mousePos.x - viewport.x)) / viewport.z - 1.0f,
			(2.0f * (mousePos.y - viewport.y)) / viewport.w - 1.0f
		);

		// Convert from NDC to clip
		glm::vec4 clipRayCoords = glm::vec4(NDC.x, NDC.y, -1.0f, 1.0f);

		// Convert from clip to eye
		glm::vec4 eyeRayCoords = glm::inverse(gameCameraCamerComp.ProjectionMatrix) * clipRayCoords;
		eyeRayCoords.z = -1.0f;
		eyeRayCoords.w = 0.0f;

		// Convert from eye to world (and then normalize)
		glm::vec4 worldRayCoords = glm::inverse(gameCameraCamerComp.ViewMatrix) * eyeRayCoords;
		glm::vec3 mouseWorldRay = normalize(glm::vec3(worldRayCoords));

		// Now we got a normalized vector from our screen position. Use this to find point in 3D space
		float div = glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), mouseWorldRay);
		float t = (1.0f - glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), gameCameraTransformComp.Position)) / div;
		glm::vec3 point3D = t * mouseWorldRay + gameCameraTransformComp.Position;

		return point3D;
	}

	void PlayerControllerSystem::LookAtPoint(const glm::vec3& point, size_t index)
	{
		// Rotate the player to look towards the mouse (point3D)
		glm::vec3 pointVector = glm::normalize(point - m_Transform[index]->Position);
		glm::vec3 originDirection = glm::vec3(0.0f, 0.0f, 1.0f);
		float extraRotation = 0.0f;
		if (point.x <= m_Transform[index]->Position.x)
		{
			originDirection.z = -1.0f;
			extraRotation = 180.0f;
		}
		float product = glm::dot(glm::normalize(originDirection), pointVector);

		float rotationOffset = glm::degrees(glm::acos(product)) + extraRotation;


		m_Transform[index]->Rotation.y = rotationOffset;
	}

	void PlayerControllerSystem::HandleMovement(size_t index)
	{
		if (Frosty::InputManager::IsKeyReleased(m_Player[index]->MoveForwardKey) || Frosty::InputManager::IsKeyReleased(m_Player[index]->MoveBackKey))
		{
			m_Physics[index]->Direction.z = 0.0f;
			m_Physics[index]->Velocity.z = 0.0f;
		}
		if (Frosty::InputManager::IsKeyReleased(m_Player[index]->MoveLeftKey) || Frosty::InputManager::IsKeyReleased(m_Player[index]->MoveRightKey))
		{
			m_Physics[index]->Direction.x = 0.0f;
			m_Physics[index]->Velocity.x = 0.0f;
		}

		if (Frosty::InputManager::IsKeyPressed(m_Player[index]->MoveForwardKey))
		{
			m_Physics[index]->Direction.z = -1.0f;
		}
		else if (Frosty::InputManager::IsKeyPressed(m_Player[index]->MoveBackKey))
		{
			m_Physics[index]->Direction.z = 1.0f;
		}
		if (Frosty::InputManager::IsKeyPressed(m_Player[index]->MoveLeftKey))
		{
			m_Physics[index]->Direction.x = -1.0f;
		}
		else if (Frosty::InputManager::IsKeyPressed(m_Player[index]->MoveRightKey))
		{
			m_Physics[index]->Direction.x = 1.0f;
		}

		if (glm::length(m_Physics[index]->Direction) > 0.0f)
		{
			m_Physics[index]->Velocity = glm::normalize(m_Physics[index]->Direction) * m_Physics[index]->Speed * m_Physics[index]->SpeedMultiplier;

			if (Frosty::InputManager::IsKeyPressed(m_Player[index]->DashKey))
			{
				// Check if entity has CDash component before publishing
				if (m_Dash[index]->CurrentCooldown <= 0.0f)
				{
					m_Dash[index]->Active = true;
					m_Physics[index]->Velocity *= m_Dash[index]->SpeedMultiplier;
					m_Dash[index]->CurrentCooldown = m_Dash[index]->COOLDOWN / 1000.0f;
				}
			}
		}
	}
	
	void PlayerControllerSystem::HandleAttack(const glm::vec3& point, size_t index)
	{
		if (Frosty::InputManager::IsKeyPressed(m_Player[index]->BasicAttackKey))
		{
			if (Frosty::Application::Get().GetWorld()->HasComponent<Frosty::ECS::CBasicAttack>(m_Transform[index]->EntityPtr))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::BasicAttackEvent>(Frosty::BasicAttackEvent(m_Transform[index]->EntityPtr));
			}
			//glm::vec3 spawnPos = m_Transform[index]->Position;
			//spawnPos.y = 1.0f;
			//auto& world = Frosty::Application::Get().GetWorld();
			//auto& projectile = world->CreateEntity(spawnPos, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f });
			//world->AddComponent<Frosty::ECS::CMesh>(projectile, Frosty::AssetManager::GetMesh("Sphere"));
			//world->AddComponent<Frosty::ECS::CMaterial>(projectile, Frosty::AssetManager::GetShader("FlatColor"));
			//auto& projectileMotion = world->AddComponent<Frosty::ECS::CMotion>(projectile, 20.0f);
			//projectileMotion.Velocity = glm::normalize(glm::vec3(point.x, 1.0f, point.z) - spawnPos) * projectileMotion.Speed;
		}
	}

	void PlayerControllerSystem::HandleInventory(size_t index)
	{
#pragma region Healing Potion
		if (Frosty::InputManager::IsKeyPressed(FY_KEY_1))
		{
				// If consumer has healing potion AND comsumer has not full health AND healing timer is bigger than cooldown--> drink healing potion
				if ((m_Inventory[index]->CurrentHealingPotions > 0) && (m_Health[index]->CurrentHealth < m_Health[index]->MaxHealth) && ((float(std::clock()) - m_Inventory[index]->HealingTimer) * 0.001f >= m_Inventory[index]->HealingCooldown))
				{
					// If healing won't exceed health capacity --> directly add heal value to health
					if (m_Inventory[index]->Heal <= (m_Health[index]->MaxHealth - m_Health[index]->CurrentHealth))
					{
						m_Health[index]->CurrentHealth += m_Inventory[index]->Heal;
					}
					// But if healing exceeds health capacity --> max health achieved
					else
					{
						m_Health[index]->CurrentHealth = m_Health[index]->MaxHealth;
					}

					// Decrease number of potions in inventory and activate the timer for cooldown
					m_Inventory[index]->CurrentHealingPotions--;
					m_Inventory[index]->HealingTimer = float(std::clock());
				}
		}
#pragma endregion Healing Potion

#pragma region Increase Health Potion
		else if (Frosty::InputManager::IsKeyPressed(FY_KEY_2))
		{
			// If consumer has increase HP potion AND comsumer can increse health AND increase HP timer is bigger than cooldown--> drink increase HP potion
			if ((m_Inventory[index]->CurrentIncreaseHPPotions > 0) && (m_Health[index]->MaxHealth < m_Health[index]->MaxPossibleHealth) && ((float(std::clock()) - m_Inventory[index]->IncreaseHPTimer) * 0.001f >= m_Inventory[index]->IncreaseHPCooldown))
			{
				// If increse HP won't exceed maximum health capacity --> directly increase health capacity 
				if (m_Inventory[index]->IncreaseHP <= (m_Health[index]->MaxPossibleHealth - m_Health[index]->MaxHealth))
				{
					m_Health[index]->MaxHealth += m_Inventory[index]->IncreaseHP;
				}
				// But if increase HP exceeds maximum health capacity --> max possible health achieved
				else
				{
					m_Health[index]->MaxHealth = m_Health[index]->MaxPossibleHealth;
				}

				// Decrease number of potions in inventory and activate the timer for cooldown
				m_Inventory[index]->CurrentIncreaseHPPotions--;
				m_Inventory[index]->IncreaseHPTimer = float(std::clock());
			}
		}
#pragma endregion Increase Health Potion

#pragma region Speed Potion
		else if (Frosty::InputManager::IsKeyPressed(FY_KEY_3))
		{
			// If consumer has speed potion AND comsumer has not full speed capacity AND speed timer is bigger than cooldown--> drink speed boost potion
			if ((m_Inventory[index]->CurrentSpeedPotions > 0) && (m_Physics[index]->Speed < m_Physics[index]->MaxSpeed) && ((float(std::clock()) - m_Inventory[index]->SpeedTimer) * 0.001f >= m_Inventory[index]->SpeedCooldown))
			{
				// If temp speed won't exceed maximum possible speed capacity --> directly add speed value to multiplier
				if (((m_Inventory[index]->IncreaseSpeedTemporary + m_Physics[index]->SpeedMultiplier) * m_Physics[index]->Speed) <= m_Physics[index]->MaxSpeed)
				{
					m_Physics[index]->SpeedMultiplier += m_Inventory[index]->IncreaseSpeedTemporary;
				}
				// But if temp speed exceeds maximum possible speed capacity --> max speed achieved (temporary) with multiplier
				else
				{
					m_Physics[index]->SpeedMultiplier = m_Physics[index]->MaxSpeed / m_Physics[index]->Speed;
				}

				// Decrease number of potions in inventory and activate the timer for cooldown
				m_Inventory[index]->CurrentSpeedPotions--;
				m_Inventory[index]->SpeedTimer = float(std::clock());
			}
		}
#pragma endregion Speed Potion

#pragma region Speed Boots
		else if (Frosty::InputManager::IsKeyPressed(FY_KEY_4))		// Appearently a temporary thing. Player picks this up automatically so onimput is unnecessary	~ W-_-W ~
		{
			// If consumer has speed potion AND comsumer has not full speed capacity
			if ((m_Inventory[index]->CurrentSpeedBoots < m_Inventory[index]->MaxSpeedBoots) && (m_Physics[index]->Speed < m_Physics[index]->MaxSpeed))
			{
				// If speed boots won't exceed maximum possible speed capacity --> directly add value to speed
				if (m_Inventory[index]->IncreaseSpeed <= (m_Physics[index]->MaxSpeed - m_Physics[index]->Speed))
				{
					m_Physics[index]->Speed += m_Inventory[index]->IncreaseSpeed;
				}
				// But if speed boots exceeds maximum possible speed capacity --> max speed achieved
				else
				{
					m_Physics[index]->Speed += m_Physics[index]->MaxSpeed - m_Physics[index]->Speed;
				}

				// Add number of boots in inventory since boots are something the entity is wearing
				m_Inventory[index]->CurrentSpeedBoots++;
			}
		}
#pragma endregion Speed Boots
	}
}