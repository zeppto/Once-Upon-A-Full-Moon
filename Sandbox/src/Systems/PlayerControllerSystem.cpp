#include <mcspch.hpp>
#include "PlayerControllerSystem.hpp"
#include "Frosty/Events/AbilityEvent.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"

namespace MCS
{
	const std::string PlayerControllerSystem::NAME = "Player Controller";

	void PlayerControllerSystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CPlayer>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CDash>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CPhysics>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CInventory>(), true);

		m_World = Frosty::Application::Get().GetWorld().get();


	}

	void PlayerControllerSystem::OnInput()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Transform[i]->Position.y != 0.0f) continue;

			// Reset (Should this be here? Maybe release instead)

			// Maybe move these functions somewhere else so they can be used by other systems as well.
			// Get the point on our terrain
			glm::vec3 point3D = ScreenToTerrainPoint();

			//Is player dead?
			if (m_Health[i]->CurrentHealth > 0)
			{
				// Look at point
				LookAtPoint(point3D, i);

				// Input
				if (!m_Dash[i]->Active)
				{
					HandleMovement(i);
					HandleAttack(i);
					HandleInventory(i);
				}
			}
			//No movement if dead
			else
			{
				m_Physics[i]->SpeedMultiplier = 0.0f;
				m_Physics[i]->Direction = glm::vec3(0.0f);
			}
		}
	}

	void PlayerControllerSystem::OnUpdate()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Dash[i]->CurrentCooldown > 0.0f) m_Dash[i]->CurrentCooldown -= Frosty::Time::DeltaTime();
			if (m_Player[i]->CurrentCooldown > 0.0f) m_Player[i]->CurrentCooldown -= Frosty::Time::DeltaTime();
			if (Frosty::Time::CurrentTime() - m_Inventory[i]->SpeedTimer >= m_Inventory[i]->SpeedCooldown && m_Physics[i]->SpeedMultiplier > 1.0f)
			{
				m_Physics[i]->SpeedMultiplier = 1.0f;
			}

			if (m_Dash[i]->Active)
			{
				m_Dash[i]->DistanceDashed += glm::length(m_Physics[i]->Direction * (m_Physics[i]->Speed * m_Dash[i]->SpeedMultiplier) * Frosty::Time::DeltaTime());
				if (m_Dash[i]->DistanceDashed >= m_Dash[i]->DISTANCE / 1000.0f)
				{
					m_Dash[i]->Active = false;
					m_Dash[i]->DistanceDashed = 0.0f;
				}
				else
				{
					m_Transform[i]->Position += glm::normalize(m_Physics[i]->Direction) * (m_Physics[i]->Speed * m_Dash[i]->SpeedMultiplier) * Frosty::Time::DeltaTime();
				}
			}
			//m_Transform[i]->Position.y = 0.f;

			UpdateHUD(i);
		}
	}

	void PlayerControllerSystem::OnEvent(Frosty::BaseEvent& e)
	{
		switch (e.GetEventType())
		{
		case Frosty::EventType::PickUp:
			OnPickUpEvent(static_cast<Frosty::PickUpEvent&>(e));
			break;
		case Frosty::EventType::UpgradeWeapon:
			OnUpgradeWeaponEvent();
			break;
		case Frosty::EventType::HealAbility:
			OnHealAbilityEvent();
			break;
		case Frosty::EventType::EnemyDeath:
			OnEnemyDeathEvent(static_cast<Frosty::EnemyDeathEvent&>(e));
			break;
		case Frosty::EventType::PlayerDamage:
			OnDamage();
			break;
		case Frosty::EventType::BossFearEffect:
			OnBossFearEffect(static_cast<Frosty::BossFearEffectEvent&>(e));
			break;
		default:
			break;
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
			m_Physics[p_Total] = &world->GetComponent<Frosty::ECS::CPhysics>(entity);
			m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);
			m_Inventory[p_Total] = &world->GetComponent<Frosty::ECS::CInventory>(entity);

			p_Total++;
		}
	}

	void PlayerControllerSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_Player[p_Total] = nullptr;
			m_Dash[p_Total] = nullptr;
			m_Physics[p_Total] = nullptr;
			m_Health[p_Total] = nullptr;
			m_Inventory[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void PlayerControllerSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CPlayer* playerPtr = world->GetComponentAddress<Frosty::ECS::CPlayer>(entity);
			Frosty::ECS::CDash* dashPtr = world->GetComponentAddress<Frosty::ECS::CDash>(entity);
			Frosty::ECS::CPhysics* physicsPtr = world->GetComponentAddress<Frosty::ECS::CPhysics>(entity);
			Frosty::ECS::CHealth* healthPtr = world->GetComponentAddress<Frosty::ECS::CHealth>(entity);
			Frosty::ECS::CInventory* inventoryPtr = world->GetComponentAddress<Frosty::ECS::CInventory>(entity);

			m_Transform[it->second] = transformPtr;
			m_Player[it->second] = playerPtr;
			m_Dash[it->second] = dashPtr;
			m_Physics[it->second] = physicsPtr;
			m_Health[it->second] = healthPtr;
			m_Inventory[it->second] = inventoryPtr;
		}
	}

	std::string PlayerControllerSystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_Player[i] << "\t" << m_Player[i]->EntityPtr->Id << "\t\t" << m_Player[i]->EntityPtr << "\t\t" << m_Player[i]->EntityPtr.use_count() << "\n";
			retInfo << "\t\t" << i << "\t" << m_Dash[i] << "\t" << m_Dash[i]->EntityPtr->Id << "\t\t" << m_Dash[i]->EntityPtr << "\t\t" << m_Dash[i]->EntityPtr.use_count() << "\n";
			retInfo << "\t\t" << i << "\t" << m_Physics[i] << "\t" << m_Physics[i]->EntityPtr->Id << "\t\t" << m_Physics[i]->EntityPtr << "\t\t" << m_Physics[i]->EntityPtr.use_count() << "\n";
			retInfo << "\t\t" << i << "\t" << m_Health[i] << "\t" << m_Health[i]->EntityPtr->Id << "\t\t" << m_Health[i]->EntityPtr << "\t\t" << m_Health[i]->EntityPtr.use_count() << "\n";
			retInfo << "\t\t" << i << "\t" << m_Inventory[i] << "\t" << m_Inventory[i]->EntityPtr->Id << "\t\t" << m_Inventory[i]->EntityPtr << "\t\t" << m_Inventory[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
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
		auto& animController = m_World->GetComponent<Frosty::ECS::CAnimController>(m_Player[index]->EntityPtr);


		if (Frosty::InputManager::IsKeyReleased(m_Player[index]->MoveForwardKey) || Frosty::InputManager::IsKeyReleased(m_Player[index]->MoveBackKey))
		{
			m_Physics[index]->Direction.z = 0.0f;
			//m_Physics[index]->Velocity.z = 0.0f;
		}
		if (Frosty::InputManager::IsKeyReleased(m_Player[index]->MoveLeftKey) || Frosty::InputManager::IsKeyReleased(m_Player[index]->MoveRightKey))
		{
			m_Physics[index]->Direction.x = 0.0f;
			//m_Physics[index]->Velocity.x = 0.0f;
		}

		if (!animController.isBusy)
		{
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
				//m_Physics[index]->Velocity = glm::normalize(m_Physics[index]->Direction) * m_Physics[index]->Speed * m_Physics[index]->SpeedMultiplier;
				m_Physics[index]->Direction = glm::normalize(m_Physics[index]->Direction);

				if (Frosty::InputManager::IsKeyPressed(m_Player[index]->DashKey))
				{
					// Check if entity has CDash component before publishing
					if (m_Dash[index]->CurrentCooldown <= 0.0f)
					{
						m_Dash[index]->Active = true;
						Frosty::EventBus::GetEventBus()->Publish<Frosty::DashEvent>(Frosty::DashEvent(m_Player[index]->EntityPtr));		

						int rand = std::rand() % 3 + 1;
						std::string str = "assets/sounds/DodgePrassel" + std::to_string(rand) + ".wav";
						const char* fileName = str.c_str();
						//Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEntityEvent>(Frosty::PlayMediaEntityEvent(m_Player[index]->EntityPtr, fileName, 5.0f, 10.0f, 100.0f, false, 0));
						Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEvent>(Frosty::PlayMediaEvent(fileName, 1.0f, 0.0f, false, 0));

						//m_Physics[index]->Velocity *= m_Dash[index]->SpeedMultiplier;
						m_Physics[index]->SpeedMultiplier = m_Dash[index]->SpeedMultiplier;
						m_Dash[index]->CurrentCooldown = m_Dash[index]->COOLDOWN / 1000.0f;
					}
				}
			}

			// Walking sounds
			if (glm::length(m_Physics[index]->Direction) > 0.0f)
			{
				if (m_Player[index]->CurrentCooldown <= 0.0f)
				{
					int rand = std::rand() % 5 + 1;
					std::string str = "assets/sounds/FootstepMud" + std::to_string(rand) + ".wav";
					const char* fileName = str.c_str();
					//Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEntityEvent>(Frosty::PlayMediaEntityEvent(m_Player[index]->EntityPtr, fileName, 1.0f, 10.0f, 100.0f, false, 0));
					Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEvent>(Frosty::PlayMediaEvent(fileName, 0.25f, 0.0f, false, 0));

					m_Player[index]->CurrentCooldown = m_Player[index]->COOLDOWN / 1000.0f;
				}
			}
		}
	}

	void PlayerControllerSystem::HandleAttack(size_t index)
	{
		// The entity pointing to a CWeapon component (player / enemy)
		auto& weaponCarrier = m_Player[index]->EntityPtr;

		// The weapon that the player is wielding
		auto& weaponComp = (m_World->GetComponent<Frosty::ECS::CWeapon>(m_Player[index]->Weapon->EntityPtr));

		if (Frosty::InputManager::IsMouseButtonPressed(m_Player[index]->LVL1Attack))
		{
			// LVL: 1, 2, 3
			if (weaponComp.Level == 1 || weaponComp.Level == 2 || weaponComp.Level == 3)
			{
				LVL1Attack(index);
			}
		}
		else if (Frosty::InputManager::IsMouseButtonPressed(m_Player[index]->LVL2Attack))
		{
			// LVL: 1, 2
			if (weaponComp.Level == 2 || weaponComp.Level == 3)
			{
				LVL2Attack(index);
			}
		}
		else if (Frosty::InputManager::IsKeyPressed(m_Player[index]->LVL3Attack))
		{
			// LVL: 1, 2, 3
			if (weaponComp.Level == 3)
			{
				LVL3Attack(index);
			}
		}
	}

	void PlayerControllerSystem::LVL1Attack(const size_t index)
	{
		// The entity pointing to a CWeapon component (player / enemy)
		auto& weaponCarrier = m_Player[index]->EntityPtr;

		// The weapon that the player is wielding
		auto& weaponComp = (m_World->GetComponent<Frosty::ECS::CWeapon>(m_Player[index]->Weapon->EntityPtr));

		// Audio setup
		int rand = std::rand() % 3 + 1;
		std::string str = "assets/sounds/SwooshLight" + std::to_string(rand) + ".wav";
		std::string bowSounds[2] = { "Short", "Long" };
		rand = std::rand() % 2 + 1;
		std::string bowStr = "assets/sounds/BowDraw" + bowSounds[rand - 1] + ".wav";
		const char* fileName = str.c_str();
		const char* fileName_Bow = bowStr.c_str();

		if (Frosty::Time::CurrentTime() - weaponComp.LVL1AttackCooldownTimer >= weaponComp.LVL1AttackCooldown - weaponComp.WindSpeed)
		{
			switch (weaponComp.Type)
			{
			case Frosty::ECS::CWeapon::WeaponType::Sword:
				//Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEntityEvent>(Frosty::PlayMediaEntityEvent(weaponCarrier, fileName, 2.0f, 30.0f, 100.0f, false, 0));
				Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEvent>(Frosty::PlayMediaEvent(fileName, 1.0f, 0.0f, false, 0));
				Frosty::EventBus::GetEventBus()->Publish <Frosty::PlayAnimEvent>(Frosty::PlayAnimEvent(weaponCarrier, 1));
				CreateLVL1BoundingBox(weaponCarrier, weaponComp.EntityPtr);
				break;
			case Frosty::ECS::CWeapon::WeaponType::Bow:
				//Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEntityEvent>(Frosty::PlayMediaEntityEvent(weaponCarrier, fileName_Bow, 2.0f, 30.0f, 100.0f, false, 0));
				Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEvent>(Frosty::PlayMediaEvent(fileName_Bow, 1.0f, 0.0f, false, 0));
				Frosty::EventBus::GetEventBus()->Publish <Frosty::PlayAnimEvent>(Frosty::PlayAnimEvent(weaponCarrier, 4));
				CreateLVL1Projectile(weaponCarrier, weaponComp.EntityPtr);
				break;
			default:
				break;
			}

			weaponComp.LVL1AttackCooldownTimer = Frosty::Time::CurrentTime();
		}
	}

	void PlayerControllerSystem::LVL2Attack(const size_t index)
	{
		// The entity pointing to a CWeapon component (player / enemy)
		auto& weaponCarrier = m_Player[index]->EntityPtr;

		// The weapon that the player is wielding
		auto& weaponComp = (m_World->GetComponent<Frosty::ECS::CWeapon>(m_Player[index]->Weapon->EntityPtr));

		// Audio setup
		int rand = std::rand() % 3 + 1;
		std::string str = "assets/sounds/SwooshLight" + std::to_string(rand) + ".wav";
		std::string bowSounds[2] = { "Short", "Long" };
		rand = std::rand() % 2 + 1;
		std::string bowStr = "assets/sounds/BowDraw" + bowSounds[rand - 1] + ".wav";
		const char* fileName = str.c_str();
		const char* fileName_Bow = bowStr.c_str();

		if (Frosty::Time::CurrentTime() - weaponComp.LVL2AttackCooldownTimer >= weaponComp.LVL2AttackCooldown - weaponComp.WindSpeed)
		{
			switch (weaponComp.Type)
			{
			case Frosty::ECS::CWeapon::WeaponType::Sword:
				//Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEntityEvent>(Frosty::PlayMediaEntityEvent(weaponCarrier, fileName, 2.0f, 30.0f, 100.0f, false, 0));
				Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEvent>(Frosty::PlayMediaEvent(fileName, 1.0f, 0.0f, false, 0));
				Frosty::EventBus::GetEventBus()->Publish <Frosty::PlayAnimEvent>(Frosty::PlayAnimEvent(weaponCarrier, 2));
				CreateLVL2BoundingBox(weaponCarrier, weaponComp.EntityPtr);
				break;
			case Frosty::ECS::CWeapon::WeaponType::Bow:
				//Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEntityEvent>(Frosty::PlayMediaEntityEvent(weaponCarrier, fileName_Bow, 2.0f, 30.0f, 100.0f, false, 0));
				Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEvent>(Frosty::PlayMediaEvent(fileName_Bow, 1.0f, 0.0f, false, 0));
				Frosty::EventBus::GetEventBus()->Publish <Frosty::PlayAnimEvent>(Frosty::PlayAnimEvent(weaponCarrier, 4));
				CreateLVL2Projectile(weaponCarrier, weaponComp.EntityPtr);
				break;
			default:
				break;
			}

			weaponComp.LVL2AttackCooldownTimer = Frosty::Time::CurrentTime();
		}
	}

	void PlayerControllerSystem::LVL3Attack(const size_t index)
	{
		// The entity pointing to a CWeapon component (player / enemy)
		auto& weaponCarrier = m_Player[index]->EntityPtr;

		// The weapon that the player is wielding
		auto& weaponComp = (m_World->GetComponent<Frosty::ECS::CWeapon>(m_Player[index]->Weapon->EntityPtr));

		// Audio setup
		int rand = std::rand() % 3 + 1;
		std::string str = "assets/sounds/SwooshLight" + std::to_string(rand) + ".wav";
		std::string bowSounds[2] = { "Short", "Long" };
		rand = std::rand() % 2 + 1;
		std::string bowStr = "assets/sounds/BowDraw" + bowSounds[rand - 1] + ".wav";
		const char* fileName = str.c_str();
		const char* fileName_Bow = bowStr.c_str();

		if (Frosty::Time::CurrentTime() - weaponComp.LVL3AttackCooldownTimer >= weaponComp.LVL3AttackCooldown - weaponComp.WindSpeed)
		{
			switch (weaponComp.Type)
			{
			case Frosty::ECS::CWeapon::WeaponType::Sword:
				//Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEntityEvent>(Frosty::PlayMediaEntityEvent(weaponCarrier, fileName, 2.0f, 30.0f, 100.0f, false, 0));
				Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEvent>(Frosty::PlayMediaEvent(fileName, 1.0f, 0.0f, false, 0));
				Frosty::EventBus::GetEventBus()->Publish <Frosty::PlayAnimEvent>(Frosty::PlayAnimEvent(weaponCarrier, 3));
				CreateLVL3BoundingBox(weaponCarrier, weaponComp.EntityPtr);
				break;
			case Frosty::ECS::CWeapon::WeaponType::Bow:
				//Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEntityEvent>(Frosty::PlayMediaEntityEvent(weaponCarrier, fileName_Bow, 2.0f, 30.0f, 100.0f, false, 0));
				Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEvent>(Frosty::PlayMediaEvent(fileName_Bow, 1.0f, 0.0f, false, 0));
				Frosty::EventBus::GetEventBus()->Publish <Frosty::PlayAnimEvent>(Frosty::PlayAnimEvent(weaponCarrier, 4));
				CreateLVL3Projectile(weaponCarrier, weaponComp.EntityPtr);
				break;
			default:
				break;
			}

			weaponComp.LVL3AttackCooldownTimer = Frosty::Time::CurrentTime();
		}
	}

	void PlayerControllerSystem::CreateLVL1BoundingBox(const std::shared_ptr<Frosty::ECS::Entity>& weaponCarrier, const std::shared_ptr<Frosty::ECS::Entity>& weapon)
	{
		// Get necessary info
		auto& weaponComp = m_World->GetComponent<Frosty::ECS::CWeapon>(weapon);
		auto& attackerTransform = m_World->GetComponent<Frosty::ECS::CTransform>(weaponCarrier);

		// Calculate direction vector
		glm::mat4 mat = glm::mat4(1.0f);
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.x), { 1.0f, 0.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.y), { 0.0f, 1.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.z), { 0.0f, 0.0f, 1.0f });
		glm::vec4 direction = mat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0);

		// Create BB
		glm::vec3 spawnPos = attackerTransform.Position + (glm::vec3(direction) * 4.0f);
		glm::vec3 spawnScale = glm::vec3(6.0f, 6.0f, 6.0f);
		auto& sword = m_World->CreateEntity({ spawnPos.x, 3.0f, spawnPos.z }, attackerTransform.Rotation + glm::vec3(0.0f, 0.0f, 90.0f), spawnScale);
		auto& swordTransform = m_World->GetComponent<Frosty::ECS::CTransform>(sword);

		float criticalHit = 0;
		criticalHit = GenerateCriticalHit(weaponComp.CriticalHit, weaponComp.CriticalHitChance + weaponComp.FireCriticalHitChance);
		int totalDamage = int(glm::round(weaponComp.Damage + criticalHit + weaponComp.EarthDamage));

		m_World->AddComponent<Frosty::ECS::CAttack>(sword, Frosty::ECS::CAttack::AttackType::Melee, totalDamage, true); // <-- true in the end because it's a friendly attack

	}

	void PlayerControllerSystem::CreateLVL2BoundingBox(const std::shared_ptr<Frosty::ECS::Entity>& weaponCarrier, const std::shared_ptr<Frosty::ECS::Entity>& weapon)
	{
		// Get necessary info
		auto& weaponComp = m_World->GetComponent<Frosty::ECS::CWeapon>(weapon);
		auto& attackerTransform = m_World->GetComponent<Frosty::ECS::CTransform>(weaponCarrier);

		// Create BB
		glm::vec3 spawnPos = attackerTransform.Position;

		glm::vec3 spawnScale = glm::vec3(15.0f, 30.0f, 15.0f);
		auto& sword = m_World->CreateEntity({ spawnPos.x, 3.0f, spawnPos.z }, { 0.f, 0.f, 0.f }, spawnScale);
		auto& swordTransform = m_World->GetComponent<Frosty::ECS::CTransform>(sword);

		float criticalHit = 0;
		criticalHit = GenerateCriticalHit(weaponComp.CriticalHit, weaponComp.CriticalHitChance + weaponComp.FireCriticalHitChance);

		int totalDamage = int(glm::round(weaponComp.Damage + criticalHit + weaponComp.EarthDamage));

		m_World->AddComponent<Frosty::ECS::CAttack>(sword, Frosty::ECS::CAttack::AttackType::Melee, totalDamage, true); // <-- true in the end because it's a friendly attack
	}

	void PlayerControllerSystem::CreateLVL3BoundingBox(const std::shared_ptr<Frosty::ECS::Entity>& weaponCarrier, const std::shared_ptr<Frosty::ECS::Entity>& weapon)
	{
		// Get necessary info
		auto& weaponComp = m_World->GetComponent<Frosty::ECS::CWeapon>(weapon);
		auto& attackerTransform = m_World->GetComponent<Frosty::ECS::CTransform>(weaponCarrier);

		// Calculate direction vector
		glm::mat4 mat = glm::mat4(1.0f);
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.x), { 1.0f, 0.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.y), { 0.0f, 1.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.z), { 0.0f, 0.0f, 1.0f });
		glm::vec4 direction = mat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0);

		// Create BB
		glm::vec3 spawnPos = attackerTransform.Position + (glm::vec3(direction) * 5.0f);
		//glm::vec3 spawnScale = glm::vec3(2.0f, 6.0f, 10.0f);
		//y needs to be the bigest if using capcol colitiondw
		glm::vec3 spawnScale = glm::vec3(3.0f, 8.0f, 3.0f);
		//the extra rotations are for rotating the bonding box
		auto& sword = m_World->CreateEntity({ spawnPos.x, 3.0f, spawnPos.z }, attackerTransform.Rotation + glm::vec3(0.0f, 90.0f, 90.0f), spawnScale);
		auto& swordTransform = m_World->GetComponent<Frosty::ECS::CTransform>(sword);

		float criticalHit = 0;
		criticalHit = GenerateCriticalHit(weaponComp.CriticalHit, weaponComp.CriticalHitChance + weaponComp.FireCriticalHitChance);

		int totalDamage = int(glm::round(weaponComp.Damage + criticalHit + weaponComp.EarthDamage));

		m_World->AddComponent<Frosty::ECS::CAttack>(sword, Frosty::ECS::CAttack::AttackType::Melee, totalDamage, true); // <-- true in the end because it's a friendly attack
	}

	void PlayerControllerSystem::CreateLVL1Projectile(const std::shared_ptr<Frosty::ECS::Entity>& weaponCarrier, const std::shared_ptr<Frosty::ECS::Entity>& weapon)
	{
		// Get necessary info
		auto& weaponComp = m_World->GetComponent<Frosty::ECS::CWeapon>(weapon);
		auto& attackerTransform = m_World->GetComponent<Frosty::ECS::CTransform>(weaponCarrier);

		// Calculate direction vector
		glm::mat4 mat = glm::mat4(1.0f);
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.x), { 1.0f, 0.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.y), { 0.0f, 1.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.z), { 0.0f, 0.0f, 1.0f });
		glm::vec4 direction = mat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0);

		// Create projectile
		glm::vec3 spawnPos = attackerTransform.Position + (glm::vec3(direction) * 3.0f);
		spawnPos.y = 5.0f;
		auto& projectile = m_World->CreateEntity({ spawnPos.x, 1.0f, spawnPos.z }, attackerTransform.Rotation, { 5.0f, 5.0f, 2.0f });
		auto& projectileTransform = m_World->GetComponent<Frosty::ECS::CTransform>(projectile);
		m_World->AddComponent<Frosty::ECS::CMesh>(projectile, Frosty::AssetManager::GetMesh("player_arrow"));
		m_World->AddComponent<Frosty::ECS::CMaterial>(projectile, Frosty::AssetManager::GetShader("Texture2D"));
		m_World->GetComponent<Frosty::ECS::CMaterial>(projectile).DiffuseTexture = Frosty::AssetManager::GetTexture2D("arrow_diffuse");
		auto& projectilePhysics = m_World->AddComponent<Frosty::ECS::CPhysics>(projectile, Frosty::AssetManager::GetBoundingBox("player_arrow"), projectileTransform.Scale, weaponComp.ProjectileSpeed);
		projectilePhysics.Direction = direction;

		auto& particles = m_World->AddComponent<Frosty::ECS::CParticleSystem>(projectile, "Particles", "particle", 50, glm::vec3(0.7f, 0.7f, 1.0f), 3.0f);
		particles.ParticleSystemDirection = glm::vec3(-1.0f, 0.0f, 0.0f);
		particles.RandomDirection = true;
		particles.randMainDir = glm::vec3(0.0f, 0.0f, -1.0f);
		particles.randSpread = 0.2f;
		particles.StartParticleSize = 0.4f;
		particles.EmitCount = 2;
		particles.EmitRate = 0.05f;
		particles.MaxLifetime = 1.5f;
		particles.FadeInTreshold = 1.4f;
		particles.FadeTreshold = 1.3f;
		particles.StaticColor = false;
		particles.SystemEndColor = glm::vec3(0.0f, 0.0f, 1.0f);
		particles.ParticleSystemStartPos = glm::vec3(0.0f, 0.0f, -0.6f);
		particles.HasGravity = true;

		float criticalHit = 0;
		criticalHit = GenerateCriticalHit(weaponComp.CriticalHit, weaponComp.CriticalHitChance + weaponComp.FireCriticalHitChance);

		int totalDamage = int(glm::round(weaponComp.Damage + criticalHit + weaponComp.EarthDamage));

		m_World->AddComponent<Frosty::ECS::CAttack>(projectile, Frosty::ECS::CAttack::AttackType::Range, totalDamage, true, weaponComp.Lifetime);
		auto& attack = m_World->GetComponent<Frosty::ECS::CAttack>(projectile);

		if (weaponComp.FireCriticalHitChance > 0.0f)
		{
			CreateFireEffect(attack, spawnPos, attackerTransform.Rotation);
		}
		if (weaponComp.EarthDamage > 0.0f)
		{
			CreateEarthEffect(attack, spawnPos, attackerTransform.Rotation);
		}
		if (weaponComp.WindSpeed > 0.0f)
		{
			CreateWindEffect(attack, spawnPos, attackerTransform.Rotation);
		}
		if (weaponComp.WaterHealing > 0.0f)
		{
			CreateWaterEffect(attack, spawnPos, attackerTransform.Rotation);
		}
	}

	void PlayerControllerSystem::CreateLVL2Projectile(const std::shared_ptr<Frosty::ECS::Entity>& weaponCarrier, const std::shared_ptr<Frosty::ECS::Entity>& weapon)
	{
		// Get necessary info
		auto& weaponComp = m_World->GetComponent<Frosty::ECS::CWeapon>(weapon);
		auto& attackerTransform = m_World->GetComponent<Frosty::ECS::CTransform>(weaponCarrier);

		glm::mat4 mat = glm::mat4(1.0f);
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.x), { 1.0f, 0.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.y), { 0.0f, 1.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.z), { 0.0f, 0.0f, 1.0f });
		glm::vec4 direction = mat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0);
		for (int i = 0; i < 3; i++)
		{
			if (i == 1)
			{
				mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.x + 10.0f), { 0.0f, 1.0f, 0.0f });
				direction = mat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0);
			}
			else if (i == 2)
			{
				mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.x + 340.0f), { 0.0f, 1.0f, 0.0f });
				direction = mat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0);
			}

			glm::vec3 spawnPos = attackerTransform.Position + (glm::vec3(direction) * 3.0f);
			spawnPos.y = 5.0f;
			auto& projectile = m_World->CreateEntity({ spawnPos.x, 1.0f, spawnPos.z }, attackerTransform.Rotation, { 5.0f, 5.0f, 2.0f });
			auto& projectileTransform = m_World->GetComponent<Frosty::ECS::CTransform>(projectile);
			m_World->AddComponent<Frosty::ECS::CMesh>(projectile, Frosty::AssetManager::GetMesh("player_arrow"));
			m_World->AddComponent<Frosty::ECS::CMaterial>(projectile, Frosty::AssetManager::GetShader("Texture2D"));
			m_World->GetComponent<Frosty::ECS::CMaterial>(projectile).DiffuseTexture = Frosty::AssetManager::GetTexture2D("arrow_diffuse");
			auto& projectilePhysics = m_World->AddComponent<Frosty::ECS::CPhysics>(projectile, Frosty::AssetManager::GetBoundingBox("player_arrow"), projectileTransform.Scale, weaponComp.ProjectileSpeed);
			projectilePhysics.Direction = direction;

			float criticalHit = 0;
			criticalHit = GenerateCriticalHit(weaponComp.CriticalHit, weaponComp.CriticalHitChance + weaponComp.FireCriticalHitChance);

			int totalDamage = int(glm::round(weaponComp.Damage + criticalHit + weaponComp.EarthDamage));

			m_World->AddComponent<Frosty::ECS::CAttack>(projectile, Frosty::ECS::CAttack::AttackType::Range, totalDamage, true, weaponComp.Lifetime);
			auto& attack = m_World->GetComponent<Frosty::ECS::CAttack>(projectile);

			auto& particles = m_World->AddComponent<Frosty::ECS::CParticleSystem>(projectile, "Particles", "particle", 50, glm::vec3(0.0f, 1.0f, 0.2f), 3.0f);
			particles.ParticleSystemDirection = glm::vec3(0.0f, 0.0f, -1.0f);
			particles.RandomDirection = true;
			particles.randMainDir = glm::vec3(0.0f, 0.0f, -1.0f);
			particles.randSpread = 0.2f;
			particles.StartParticleSize = 0.4f;
			particles.EmitCount = 2;
			particles.EmitRate = 0.05f;
			particles.MaxLifetime = 1.5f;
			particles.FadeInTreshold = 1.4f;
			particles.FadeTreshold = 1.3f;
			particles.StaticColor = false;
			particles.SystemEndColor = glm::vec3(1.0f, 1.0f, 1.0f);
			particles.ParticleSystemStartPos = glm::vec3(0.0f, 0.0f, -0.6f);
			particles.HasGravity = true;

			if (weaponComp.FireCriticalHitChance > 0.0f)
			{
				CreateFireEffect(attack, spawnPos, attackerTransform.Rotation);
			}
			if (weaponComp.EarthDamage > 0.0f)
			{
				CreateEarthEffect(attack, spawnPos, attackerTransform.Rotation);
			}
			if (weaponComp.WindSpeed > 0.0f)
			{
				CreateWindEffect(attack, spawnPos, attackerTransform.Rotation);
			}
			if (weaponComp.WaterHealing > 0.0f)
			{
				CreateWaterEffect(attack, spawnPos, attackerTransform.Rotation);
			}
		}
	}

	void PlayerControllerSystem::CreateLVL3Projectile(const std::shared_ptr<Frosty::ECS::Entity>& weaponCarrier, const std::shared_ptr<Frosty::ECS::Entity>& weapon)
	{
		// Get necessary info
		auto& weaponComp = m_World->GetComponent<Frosty::ECS::CWeapon>(weapon);
		auto& attackerTransform = m_World->GetComponent<Frosty::ECS::CTransform>(weaponCarrier);

		// Calculate direction vector
		glm::mat4 mat = glm::mat4(1.0f);
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.x), { 1.0f, 0.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.y), { 0.0f, 1.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.z), { 0.0f, 0.0f, 1.0f });
		glm::vec4 direction = mat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0);

		// Create projectile
		glm::vec3 spawnPos = attackerTransform.Position + (glm::vec3(direction) * 3.0f);
		spawnPos.y = 5.0f;
		auto& projectile = m_World->CreateEntity({ spawnPos.x, 1.0f, spawnPos.z }, attackerTransform.Rotation, { 5.0f, 5.0f, 2.0f });
		auto& projectileTransform = m_World->GetComponent<Frosty::ECS::CTransform>(projectile);
		m_World->AddComponent<Frosty::ECS::CMesh>(projectile, Frosty::AssetManager::GetMesh("player_arrow"));
		m_World->AddComponent<Frosty::ECS::CMaterial>(projectile, Frosty::AssetManager::GetShader("Texture2D"));
		m_World->GetComponent<Frosty::ECS::CMaterial>(projectile).DiffuseTexture = Frosty::AssetManager::GetTexture2D("arrow_diffuse");
		auto& projectilePhysics = m_World->AddComponent<Frosty::ECS::CPhysics>(projectile, Frosty::AssetManager::GetBoundingBox("player_arrow"), projectileTransform.Scale, weaponComp.ProjectileSpeed);
		projectilePhysics.Direction = direction;

		float criticalHit = 0;
		criticalHit = GenerateCriticalHit(weaponComp.CriticalHit, weaponComp.CriticalHitChance + weaponComp.FireCriticalHitChance);

		int totalDamage = int(glm::round(weaponComp.Damage + criticalHit + weaponComp.EarthDamage));

		m_World->AddComponent<Frosty::ECS::CAttack>(projectile, Frosty::ECS::CAttack::AttackType::Range, totalDamage, true, weaponComp.Lifetime, false);
		auto& attack = m_World->GetComponent<Frosty::ECS::CAttack>(projectile);

		auto& particles = m_World->AddComponent<Frosty::ECS::CParticleSystem>(projectile, "Particles", "particle", 50, glm::vec3(0.0f, 1.0f, 0.2f), 3.0f);
		particles.ParticleSystemDirection = glm::vec3(0.0f, 0.0f, -1.0f);
		particles.RandomDirection = true;
		particles.randMainDir = glm::vec3(0.0f, 0.0f, -1.0f);
		particles.randSpread = 0.2f;
		particles.StartParticleSize = 0.4f;
		particles.EmitCount = 2;
		particles.EmitRate = 0.05f;
		particles.MaxLifetime = 1.5f;
		particles.FadeInTreshold = 1.4f;
		particles.FadeTreshold = 1.3f;
		particles.StaticColor = false;
		particles.SystemEndColor = glm::vec3(1.0f, 1.0f, 1.0f);
		particles.ParticleSystemStartPos = glm::vec3(0.0f, 0.0f, -0.6f);
		particles.HasGravity = true;

		if (weaponComp.FireCriticalHitChance > 0.0f)
		{
			CreateFireEffect(attack, spawnPos, attackerTransform.Rotation);
		}
		if (weaponComp.EarthDamage > 0.0f)
		{
			CreateEarthEffect(attack, spawnPos, attackerTransform.Rotation);
		}
		if (weaponComp.WindSpeed > 0.0f)
		{
			CreateWindEffect(attack, spawnPos, attackerTransform.Rotation);
		}
		if (weaponComp.WaterHealing > 0.0f)
		{
			CreateWaterEffect(attack, spawnPos, attackerTransform.Rotation);
		}
	}

	void PlayerControllerSystem::CreateFireEffect(Frosty::ECS::CAttack& attack, glm::vec3 spawnPos, glm::vec3 rotation)
	{
		auto& fireEffect = m_World->CreateEntity({ spawnPos.x, 1.0f, spawnPos.z }, rotation, { 5.0f, 5.0f, 2.0f });

		auto& fireParticles = m_World->AddComponent<Frosty::ECS::CParticleSystem>(fireEffect, "Particles", "particleSpark1", 30, glm::vec3(1.0f, 0.0f, 0.0f), 3.0f);
		fireParticles.ParticleSystemDirection = glm::vec3(-1.0f, 0.0f, 0.0f);
		fireParticles.RandomDirection = true;
		fireParticles.randMainDir = glm::vec3(0.0f, 0.0f, -1.0f);
		fireParticles.randSpread = 0.05f;
		fireParticles.StartParticleSize = 0.4f;
		fireParticles.EmitCount = 2;
		fireParticles.EmitRate = 0.05f;
		fireParticles.MaxLifetime = 1.5f;
		fireParticles.FadeInTreshold = 1.4f;
		fireParticles.FadeTreshold = 1.3f;
		fireParticles.StaticColor = false;
		fireParticles.SystemEndColor = glm::vec3(0.0f, 0.0f, 1.0f);
		fireParticles.ParticleSystemStartPos = glm::vec3(0.0f, 0.0f, -0.6f);
		fireParticles.HasGravity = true;

		attack.FireEffect = fireEffect;
	}

	void PlayerControllerSystem::CreateEarthEffect(Frosty::ECS::CAttack& attack, glm::vec3 spawnPos, glm::vec3 rotation)
	{
		auto& earthEffect = m_World->CreateEntity({ spawnPos.x, 1.0f, spawnPos.z }, rotation, { 5.0f, 5.0f, 2.0f });

		auto& earthParticles = m_World->AddComponent<Frosty::ECS::CParticleSystem>(earthEffect, "Particles", "particleSpark1", 30, glm::vec3(0.5f, 0.5f, 0.1f), 3.0f);
		earthParticles.ParticleSystemDirection = glm::vec3(-1.0f, 0.0f, 0.0f);
		earthParticles.RandomDirection = true;
		earthParticles.randMainDir = glm::vec3(0.0f, 0.0f, -1.0f);
		earthParticles.randSpread = 0.05f;
		earthParticles.StartParticleSize = 0.4f;
		earthParticles.EmitCount = 2;
		earthParticles.EmitRate = 0.05f;
		earthParticles.MaxLifetime = 1.5f;
		earthParticles.FadeInTreshold = 1.4f;
		earthParticles.FadeTreshold = 1.3f;
		earthParticles.StaticColor = false;
		earthParticles.SystemEndColor = glm::vec3(0.0f, 0.0f, 1.0f);
		earthParticles.ParticleSystemStartPos = glm::vec3(0.0f, 0.0f, -0.6f);
		earthParticles.HasGravity = true;

		attack.EarthEffect = earthEffect;
	}

	void PlayerControllerSystem::CreateWindEffect(Frosty::ECS::CAttack& attack, glm::vec3 spawnPos, glm::vec3 rotation)
	{
		auto& windEffect = m_World->CreateEntity({ spawnPos.x, 1.0f, spawnPos.z }, rotation, { 5.0f, 5.0f, 2.0f });

		auto& windParticles = m_World->AddComponent<Frosty::ECS::CParticleSystem>(windEffect, "Particles", "particleSpark1", 30, glm::vec3(0.0f, 1.0f, 0.0f), 3.0f);
		windParticles.ParticleSystemDirection = glm::vec3(-1.0f, 0.0f, 0.0f);
		windParticles.RandomDirection = true;
		windParticles.randMainDir = glm::vec3(0.0f, 0.0f, -1.0f);
		windParticles.randSpread = 0.05f;
		windParticles.StartParticleSize = 0.4f;
		windParticles.EmitCount = 2;
		windParticles.EmitRate = 0.05f;
		windParticles.MaxLifetime = 1.5f;
		windParticles.FadeInTreshold = 1.4f;
		windParticles.FadeTreshold = 1.3f;
		windParticles.StaticColor = false;
		windParticles.SystemEndColor = glm::vec3(0.0f, 0.0f, 1.0f);
		windParticles.ParticleSystemStartPos = glm::vec3(0.0f, 0.0f, -0.6f);
		windParticles.HasGravity = true;

		attack.WindEffect = windEffect;
	}

	void PlayerControllerSystem::CreateWaterEffect(Frosty::ECS::CAttack& attack, glm::vec3 spawnPos, glm::vec3 rotation)
	{
		auto& waterEffect = m_World->CreateEntity({ spawnPos.x, 1.0f, spawnPos.z }, rotation, { 5.0f, 5.0f, 2.0f });

		auto& waterParticles = m_World->AddComponent<Frosty::ECS::CParticleSystem>(waterEffect, "Particles", "particleSpark1", 30, glm::vec3(0.0f, 0.0f, 1.0f), 3.0f);
		waterParticles.ParticleSystemDirection = glm::vec3(-1.0f, 0.0f, 0.0f);
		waterParticles.RandomDirection = true;
		waterParticles.randMainDir = glm::vec3(0.0f, 0.0f, -1.0f);
		waterParticles.randSpread = 0.05f;
		waterParticles.StartParticleSize = 0.4f;
		waterParticles.EmitCount = 2;
		waterParticles.EmitRate = 0.05f;
		waterParticles.MaxLifetime = 1.5f;
		waterParticles.FadeInTreshold = 1.4f;
		waterParticles.FadeTreshold = 1.3f;
		waterParticles.StaticColor = false;
		waterParticles.SystemEndColor = glm::vec3(0.0f, 0.0f, 1.0f);
		waterParticles.ParticleSystemStartPos = glm::vec3(0.0f, 0.0f, -0.6f);
		waterParticles.HasGravity = true;

		attack.WaterEffect = waterEffect;
	}

	float PlayerControllerSystem::GenerateCriticalHit(float criticalHit, float criticalHitChance)
	{
		int randomNumber = rand() % 100 + 1;

		if (randomNumber <= (criticalHitChance * 100))
		{
			return criticalHit;
		}
		return 0.0f;
	}

	void PlayerControllerSystem::HandleInventory(size_t index)
	{
#pragma region Healing Potion
		if (Frosty::InputManager::IsKeyPressed(m_Player[index]->HealingPotionKey))
		{
			// If consumer has healing potion AND comsumer has not full health AND healing timer is bigger than cooldown--> drink healing potion
			if ((m_Inventory[index]->CurrentHealingPotions > 0) && (m_Health[index]->CurrentHealth < m_Health[index]->MaxHealth) && (Frosty::Time::CurrentTime() - m_Inventory[index]->HealingTimer >= m_Inventory[index]->HealingCooldown))
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
				m_Inventory[index]->HealingTimer = Frosty::Time::CurrentTime();

				// Drinking potion
				//Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEvent>(Frosty::PlayMediaEvent("", 2.0f, false, 0));
			}
		}
#pragma endregion Healing Potion

		//#pragma region Increase Health Potion
		//		else if (Frosty::InputManager::IsKeyPressed(m_Player[index]->IncreaseHPPotionKey))
		//		{
		//			// If consumer has increase HP potion AND comsumer can increse health AND increase HP timer is bigger than cooldown--> drink increase HP potion
		//			if ((m_Inventory[index]->CurrentIncreaseHPPotions > 0) && (m_Health[index]->MaxHealth < m_Health[index]->MaxPossibleHealth) && (Frosty::Time::CurrentTime() - m_Inventory[index]->IncreaseHPTimer >= m_Inventory[index]->IncreaseHPCooldown))
		//			{
		//				// If increse HP won't exceed maximum health capacity --> directly increase health capacity 
		//				if (m_Inventory[index]->IncreaseHP <= (m_Health[index]->MaxPossibleHealth - m_Health[index]->MaxHealth))
		//				{
		//					m_Health[index]->MaxHealth += m_Inventory[index]->IncreaseHP;
		//				}
		//				// But if increase HP exceeds maximum health capacity --> max possible health achieved
		//				else
		//				{
		//					m_Health[index]->MaxHealth = m_Health[index]->MaxPossibleHealth;
		//				}
		//
		//				// Decrease number of potions in inventory and activate the timer for cooldown
		//				m_Inventory[index]->CurrentIncreaseHPPotions--;
		//				m_Inventory[index]->IncreaseHPTimer = Frosty::Time::CurrentTime();
		//			}
		//		}
		//#pragma endregion Increase Health Potion

#pragma region Speed Potion
		else if (Frosty::InputManager::IsKeyPressed(m_Player[index]->SpeedPotionKey))
		{
			// If consumer has speed potion AND comsumer has not full speed capacity AND speed timer is bigger than cooldown--> drink speed boost potion
			if ((m_Inventory[index]->CurrentSpeedPotions > 0) && (m_Physics[index]->Speed < m_Physics[index]->MaxSpeed) && (Frosty::Time::CurrentTime() - m_Inventory[index]->SpeedTimer >= m_Inventory[index]->SpeedCooldown))
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
				m_Inventory[index]->SpeedTimer = Frosty::Time::CurrentTime();

				// Drinking potion
				//Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEvent>(Frosty::PlayMediaEvent("", 2.0f, false, 0));
			}
		}
#pragma endregion Speed Potion

		//#pragma region Speed Boots
		//		else if (Frosty::InputManager::IsKeyPressed(m_Player[index]->SpeedBootsKey))		// Appearently a temporary thing. Player picks this up automatically so onimput is unnecessary	~ W-_-W ~
		//		{
		//			// If consumer has speed potion AND comsumer has not full speed capacity
		//			if ((m_Inventory[index]->CurrentSpeedBoots < m_Inventory[index]->MaxSpeedBoots) && (m_Physics[index]->Speed < m_Physics[index]->MaxSpeed))
		//			{
		//				// If speed boots won't exceed maximum possible speed capacity --> directly add value to speed
		//				if (m_Inventory[index]->IncreaseSpeed <= (m_Physics[index]->MaxSpeed - m_Physics[index]->Speed))
		//				{
		//					m_Physics[index]->Speed += m_Inventory[index]->IncreaseSpeed;
		//				}
		//				// But if speed boots exceeds maximum possible speed capacity --> max speed achieved
		//				else
		//				{
		//					m_Physics[index]->Speed += m_Physics[index]->MaxSpeed - m_Physics[index]->Speed;
		//				}
		//
		//				// Add number of boots in inventory since boots are something the entity is wearing
		//				m_Inventory[index]->CurrentSpeedBoots++;
		//			}
		//		}
		//#pragma endregion Speed Boots

#pragma region Pick Up
		if (Frosty::InputManager::IsKeyPressed(m_Player[index]->InteractionKey))
		{
			if (!m_keyPressed)
			{
				m_keyPressed = true;
				Frosty::EventBus::GetEventBus()->Publish<Frosty::PickUpAttemptEvent>(Frosty::PickUpAttemptEvent(m_Player[index]->EntityPtr));

				// Send event to start the hexCircle timer (send the one requesting the enchantment)
				Frosty::EventBus::GetEventBus()->Publish<Frosty::ActivateWitchCircleEvent>(Frosty::ActivateWitchCircleEvent(m_Player[index]->EntityPtr));
			}
		}
		else if (m_keyPressed)
		{
			m_keyPressed = false;
		}
#pragma endregion Pick Up

#pragma region Bait
		else if (Frosty::InputManager::IsKeyPressed(m_Player[index]->DropBaitKey))
		{
			// If player has bait AND bait timer is bigger than cooldown--> Lay down bait
			if ((m_Inventory[index]->CurrentBaitAmount > 0) && (Frosty::Time::CurrentTime() - m_Inventory[index]->BaitTimer >= m_Inventory[index]->BaitCooldown))
			{
				auto& world = Frosty::Application::Get().GetWorld();
				auto& bait = world->CreateEntity();
				world->AddComponent<Frosty::ECS::CMesh>(bait, Frosty::AssetManager::GetMesh("meat"));
				auto& material = world->AddComponent<Frosty::ECS::CMaterial>(bait, Frosty::AssetManager::GetShader("Texture2D"));
				material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("meat");
				auto& transform = world->GetComponent<Frosty::ECS::CTransform>(bait);
				transform.Position = m_Transform[index]->Position;

				//Send event with bait pos to boss
				Frosty::EventBus::GetEventBus()->Publish<Frosty::BaitPlacedEvent>(Frosty::BaitPlacedEvent(bait));

				m_Inventory[index]->CurrentBaitAmount--;
				m_Inventory[index]->BaitTimer = Frosty::Time::CurrentTime();
				
				// Drop meat (walking sound?)
				//Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEvent>(Frosty::PlayMediaEvent("", 2.0f, false, 0));
			}
		}
#pragma endregion Bait
	}

	void PlayerControllerSystem::OnPickUpEvent(Frosty::PickUpEvent& e)
	{
		auto world = Frosty::Application::Get().GetWorld().get();
		for (size_t i = 1; i < p_Total; i++)
		{
			auto& loot = world->GetComponent<Frosty::ECS::CLootable>(e.GetEntity());
			auto& HUD = m_World->GetComponent<Frosty::ECS::CGUI>(m_Transform[i]->EntityPtr);

			if (loot.Type == Frosty::ECS::CLootable::LootType::HealingPotion)
			{
				if (m_Inventory[i]->CurrentHealingPotions < m_Inventory[i]->MaxHealingPotions)
				{
					m_Inventory[i]->CurrentHealingPotions++;
					SetPickUpText(i, "+1 Healing Potion");

					FY_INFO("HealingPotion in Inventory");
					FY_INFO("{0} / {1}", m_Inventory[i]->CurrentHealingPotions, m_Inventory[i]->MaxHealingPotions);
					if (!world->HasComponent<Frosty::ECS::CDestroy>(e.GetEntity()))
					{
						world->AddComponent<Frosty::ECS::CDestroy>(e.GetEntity());
					}
				}
				else
				{
					SetPickUpText(i, "Can't Pick Up Healing Potion");
				}

			}
			else if (loot.Type == Frosty::ECS::CLootable::LootType::IncHealthPotion)
			{
				if (m_Inventory[i]->CurrentIncreaseHPPotions < m_Inventory[i]->MaxIncreaseHPPotions && m_Health[i]->MaxHealth < m_Health[i]->MaxPossibleHealth)
				{
					// If increse HP won't exceed maximum health capacity --> directly increase health capacity 
					if (m_Inventory[i]->IncreaseHP <= (m_Health[i]->MaxPossibleHealth - m_Health[i]->MaxHealth))
					{
						m_Health[i]->MaxHealth += m_Inventory[i]->IncreaseHP;
					}
					// But if increase HP exceeds maximum health capacity --> max possible health achieved
					else
					{
						m_Health[i]->MaxHealth = m_Health[i]->MaxPossibleHealth;
					}
					m_Health[i]->CurrentHealth += 4;
					SetPickUpText(i, "Max Health Increased");

					FY_INFO("Max Health Increased");
					//FY_INFO("{0} / {1}", m_Inventory[i]->CurrentIncreaseHPPotions, m_Inventory[i]->MaxIncreaseHPPotions);
					if (!world->HasComponent<Frosty::ECS::CDestroy>(e.GetEntity()))
					{
						world->AddComponent<Frosty::ECS::CDestroy>(e.GetEntity());
					}
				}
				else
				{
					SetPickUpText(i, "Can't Pick Up Max Health Increaser");
				}
			}
			else if (loot.Type == Frosty::ECS::CLootable::LootType::SpeedPotion)
			{
				if (m_Inventory[i]->CurrentSpeedPotions < m_Inventory[i]->MaxSpeedPotions)
				{
					m_Inventory[i]->CurrentSpeedPotions++;
					SetPickUpText(i, "+1 Speed Potion");

					FY_INFO("SpeedPotion in Inventory");
					FY_INFO("{0} / {1}", m_Inventory[i]->CurrentSpeedPotions, m_Inventory[i]->MaxSpeedPotions);
					if (!world->HasComponent<Frosty::ECS::CDestroy>(e.GetEntity()))
					{
						world->AddComponent<Frosty::ECS::CDestroy>(e.GetEntity());
					}
				}
				else
				{
					SetPickUpText(i, "Can't Pick Up Speed Potion");
				}
			}
			else if (loot.Type == Frosty::ECS::CLootable::LootType::SpeedBoots)
			{
				if (m_Inventory[i]->CurrentSpeedBoots < m_Inventory[i]->MaxSpeedBoots)
				{
					if (m_Inventory[i]->IncreaseSpeed <= (m_Physics[i]->MaxSpeed - m_Physics[i]->Speed))
					{
						m_Inventory[i]->CurrentSpeedBoots++;
						m_Physics[i]->Speed += m_Inventory[i]->IncreaseSpeed;
						SetPickUpText(i, "Speed Increased");
					}
					else
					{
						m_Physics[i]->Speed = m_Physics[i]->MaxSpeed;
					}
					FY_INFO("SpeedBoots Activated");
					FY_INFO("{0} / {1}", m_Inventory[i]->CurrentSpeedBoots, m_Inventory[i]->MaxSpeedBoots);
					if (!world->HasComponent<Frosty::ECS::CDestroy>(e.GetEntity()))
					{
						world->AddComponent<Frosty::ECS::CDestroy>(e.GetEntity());
					}
				}
			}
			else if (loot.Type == Frosty::ECS::CLootable::LootType::Wolfsbane)
			{
				if (m_Inventory[i]->CurrentWolfsbane < m_Inventory[i]->MaxWolfsbaneAmount)
				{
					m_Inventory[i]->CurrentWolfsbane++;
					SetPickUpText(i, "+1 Wolfsbane");

					FY_INFO("Wolfsbane in Inventory");
					FY_INFO("{0} / {1}", m_Inventory[i]->CurrentSpeedPotions, m_Inventory[i]->MaxSpeedPotions);

					if (!world->HasComponent<Frosty::ECS::CDestroy>(e.GetEntity()))
					{
						world->AddComponent<Frosty::ECS::CDestroy>(e.GetEntity());
					}
				}
				else
					SetPickUpText(i, "Can't Pick Up Wolfsbane");
			}
			else if (loot.Type == Frosty::ECS::CLootable::LootType::Bait)
			{
				if (m_Inventory[i]->CurrentBaitAmount < m_Inventory[i]->MaxBaitAmount)
				{
					m_Inventory[i]->CurrentBaitAmount++;
					SetPickUpText(i, "+1 Bait");

					FY_INFO("Bait in Inventory");
					FY_INFO("{0} / {1}", m_Inventory[i]->CurrentSpeedPotions, m_Inventory[i]->MaxSpeedPotions);

					if (!world->HasComponent<Frosty::ECS::CDestroy>(e.GetEntity()))
					{
						world->AddComponent<Frosty::ECS::CDestroy>(e.GetEntity());
					}
				}
				else
					SetPickUpText(i, "Can't Pick Up Bait");
			}
			else if (loot.Type == Frosty::ECS::CLootable::LootType::Weapon)
			{
				FY_INFO("Weapon");
				SwapWeapon(m_Player[i]->Weapon->EntityPtr, e.GetEntity());
				ResetAllHUDWeaponInfo(i);

				auto& playerWeapon = m_World->GetComponent<Frosty::ECS::CWeapon>(m_Player[i]->Weapon->EntityPtr);

				if (playerWeapon.Type == Frosty::ECS::CWeapon::WeaponType::Sword)
				{
					if (playerWeapon.Level == 1)
						SetPickUpText(i, "Picked Up Sword Level 1");
					else if (playerWeapon.Level == 2)
						SetPickUpText(i, "Picked Up Sword Level 2");
					else if (playerWeapon.Level == 3)
						SetPickUpText(i, "Picked Up Sword Level 3");

					HUD.Layout.sprites.at(1).SetImage("attackMelee");
					HUD.Layout.sprites.at(2).SetImage("attackMelee1");
					HUD.Layout.sprites.at(3).SetImage("attackMelee2");
					HUD.Layout.sprites.at(4).SetImage("attackMelee3");
					
					// Audio setup
					int rand = std::rand() % 2 + 1;
					std::string str = "assets/sounds/WeaponPickup" + std::to_string(rand) + ".wav";
					const char* fileName = str.c_str();
					Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEntityEvent>(Frosty::PlayMediaEntityEvent(loot.EntityPtr, fileName, 2.0f, 30.0f, 100.0f, false, 0));
				}
				else if (playerWeapon.Type == Frosty::ECS::CWeapon::WeaponType::Bow)
				{
					if (playerWeapon.Level == 1)
						SetPickUpText(i, "Picked Up Bow Level 1");
					else if (playerWeapon.Level == 2)
						SetPickUpText(i, "Picked Up Bow Level 2");
					else if (playerWeapon.Level == 3)
						SetPickUpText(i, "Picked Up Bow Level 3");

					// Audio setup
					std::string bowSounds[2] = { "Short", "Long" };
					int rand = std::rand() % 2 + 1;
					std::string bowStr = "assets/sounds/BowDraw" + bowSounds[rand - 1] + ".wav";
					const char* fileName_Bow = bowStr.c_str();
					Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEntityEvent>(Frosty::PlayMediaEntityEvent(loot.EntityPtr, fileName_Bow, 2.0f, 30.0f, 100.0f, false, 0));

					HUD.Layout.sprites[2].SetImage("attackRanged");
					HUD.Layout.sprites[3].SetImage("attackRanged1");
					HUD.Layout.sprites[4].SetImage("attackRanged2");
					HUD.Layout.sprites[5].SetImage("attackRanged3");
				}


			}
		}
	}

	void PlayerControllerSystem::OnUpgradeWeaponEvent()
	{
		auto& weaponComp = m_World->GetComponent<Frosty::ECS::CWeapon>(m_Player[p_Total - 1]->Weapon->EntityPtr);

		// If player weapon is NOT fully upgraded --> proceed
		if (!weaponComp.IsFullyUpgraded)
		{
			auto& HUD = m_World->GetComponent<Frosty::ECS::CGUI>(m_Transform[p_Total - 1]->EntityPtr);


			// Each element is represented by a number 0-3 (Fire, Earth, Wind, Water)
			std::vector<int> elements;
			elements.reserve(4);

			// Check which elements are possible to generate
			if (weaponComp.FireCriticalHitChance == 0.f)	// Fire = 0
				elements.emplace_back(0);
			if (weaponComp.EarthDamage == 0.f)				// Earth = 1
				elements.emplace_back(1);
			if (weaponComp.WindSpeed == 0.f)				// Wind = 2
				elements.emplace_back(2);
			if (weaponComp.WaterHealing == 0.f)				// Water = 3
				elements.emplace_back(3);

			int randomElement = rand() % int(elements.size());

			// Upgrade according to randomized value
			switch (elements[randomElement])
			{
			case 0:
				if (weaponComp.FireCriticalHitChance == 0.f)
				{

					weaponComp.FireCriticalHitChance += 0.1f;
					HUD.Layout.sprites[7].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					//HUD.Layout.sprites.at(6).SetTranslateSprite(glm::vec2(960 / 1.5f, (540 / 1.5f) + 500));
					//HUD.Layout.sprites.at(6).SetScaleSprite(glm::vec2(1.5, 1.5));
					m_Player[p_Total - 1]->ElementDisplayTimer = Frosty::Time::CurrentTime();
					SetPickUpText(p_Total - 1, "Chance of double damage");


				}
				break;
			case 1:
				if (weaponComp.EarthDamage == 0.f)
				{
					weaponComp.EarthDamage += 1.f;

					HUD.Layout.sprites[6].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					//HUD.Layout.sprites.at(5).SetTranslateSprite(glm::vec2(960 / 1.5f, (540 / 1.5f) + 500));
					//HUD.Layout.sprites.at(5).SetScaleSprite(glm::vec2(1.5, 1.5));
					m_Player[p_Total - 1]->ElementDisplayTimer = Frosty::Time::CurrentTime();
					SetPickUpText(p_Total - 1, "Increased damage");


				}
				break;
			case 2:
				if (weaponComp.WindSpeed == 0.f)
				{
					weaponComp.WindSpeed += 0.4f;

					HUD.Layout.sprites[9].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					//HUD.Layout.sprites.at(8).SetTranslateSprite(glm::vec2(960 / 1.5f, (540 / 1.5f) + 500));
					//HUD.Layout.sprites.at(8).SetScaleSprite(glm::vec2(1.5, 1.5));
					m_Player[p_Total - 1]->ElementDisplayTimer = Frosty::Time::CurrentTime();
					SetPickUpText(p_Total - 1, "Increased attack speed");


				}
				break;
			case 3:
				if (weaponComp.WaterHealing == 0)
				{
					weaponComp.WaterHealing += 1;

					HUD.Layout.sprites[8].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					//HUD.Layout.sprites.at(7).SetTranslateSprite(glm::vec2(960 / 1.5f, (540 / 1.5f) + 500));
					//HUD.Layout.sprites.at(7).SetScaleSprite(glm::vec2(1.5, 1.5));
					m_Player[p_Total - 1]->ElementDisplayTimer = Frosty::Time::CurrentTime();
					SetPickUpText(p_Total - 1, "Chance of heal on hit");



				}
				break;
			default:
				break;
			}

			m_Player[p_Total - 1]->ElementMoveTimer = Frosty::Time::CurrentTime();

			auto& inventoryComp = m_World->GetComponent<Frosty::ECS::CInventory>(m_Player[p_Total - 1]->EntityPtr);
			inventoryComp.CurrentWolfsbane--;

			// If all four elements are upgraded --> weapon becomes fully upgraded
			if (weaponComp.FireCriticalHitChance != 0.f && weaponComp.EarthDamage != 0.f && weaponComp.WindSpeed != 0.f && weaponComp.WaterHealing != 0)
				weaponComp.IsFullyUpgraded = true;




		}
	}

	void PlayerControllerSystem::OnHealAbilityEvent()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			auto& weaponComp = m_World->GetComponent<Frosty::ECS::CWeapon>(m_Player[i]->Weapon->EntityPtr);

			if (m_Health[i]->CurrentHealth < m_Health[i]->MaxHealth && weaponComp.WaterHealing != 0)
			{
				uint8_t randomValue = rand() % 10 + 1;

				// 50% chance of healing
				if (randomValue <= 5)
				{
					// If healing won't exceed health capacity --> directly add heal value to health
					if (weaponComp.WaterHealing <= (m_Health[i]->MaxHealth - m_Health[i]->CurrentHealth))
					{
						m_Health[i]->CurrentHealth += weaponComp.WaterHealing;
					}
					// But if healing exceeds health capacity --> max health achieved
					else
					{
						m_Health[i]->CurrentHealth = m_Health[i]->MaxHealth;
					}
				}
			}
		}
	}

	void PlayerControllerSystem::OnEnemyDeathEvent(Frosty::EnemyDeathEvent& e)
	{
		m_Player[1]->Score += e.GetPoints();
	}

	void PlayerControllerSystem::SwapWeapon(const std::shared_ptr<Frosty::ECS::Entity>& playerWeapon, const std::shared_ptr<Frosty::ECS::Entity>& lootWeapon)
	{
		// Swap CWeapon
		if ((m_World->HasComponent<Frosty::ECS::CWeapon>(playerWeapon)) && (m_World->HasComponent<Frosty::ECS::CWeapon>(lootWeapon)))
		{
			// Swap loot type in lootWeapon depending on playerWeapon
			SwapLootType(playerWeapon, lootWeapon);

			auto& playerWeaponComp = m_World->GetComponent<Frosty::ECS::CWeapon>(playerWeapon);
			auto& lootWeaponComp = m_World->GetComponent<Frosty::ECS::CWeapon>(lootWeapon);

			Frosty::ECS::CWeapon::WeaponType type = playerWeaponComp.Type;
			uint8_t level = playerWeaponComp.Level;
			std::string speciality = playerWeaponComp.Speciality;
			float maxAttackRange = playerWeaponComp.MaxAttackRange;
			float minAttackRange = playerWeaponComp.MinAttackRange;
			float damage = playerWeaponComp.Damage;
			float criticalHit = playerWeaponComp.CriticalHit;
			float criticalHitChance = playerWeaponComp.CriticalHitChance;
			float lvl1Cooldown = playerWeaponComp.LVL1AttackCooldown;
			float lvl2Cooldown = playerWeaponComp.LVL2AttackCooldown;
			float lvl3Cooldown = playerWeaponComp.LVL3AttackCooldown;
			float lifetime = playerWeaponComp.Lifetime;
			glm::vec3 attackHitboxScale = playerWeaponComp.AttackHitboxScale;
			float fire = playerWeaponComp.FireCriticalHitChance;
			float earth = playerWeaponComp.EarthDamage;
			float wind = playerWeaponComp.WindSpeed;
			int water = playerWeaponComp.WaterHealing;
			bool upgraded = playerWeaponComp.IsFullyUpgraded;
			float projectileSpeed = playerWeaponComp.ProjectileSpeed;

			playerWeaponComp.Type = lootWeaponComp.Type;
			playerWeaponComp.Level = lootWeaponComp.Level;
			playerWeaponComp.Speciality = lootWeaponComp.Speciality;
			playerWeaponComp.MaxAttackRange = lootWeaponComp.MaxAttackRange;
			playerWeaponComp.MinAttackRange = lootWeaponComp.MinAttackRange;
			playerWeaponComp.Damage = lootWeaponComp.Damage;
			playerWeaponComp.CriticalHit = lootWeaponComp.CriticalHit;
			playerWeaponComp.CriticalHitChance = lootWeaponComp.CriticalHitChance;
			playerWeaponComp.LVL1AttackCooldown = lootWeaponComp.LVL1AttackCooldown;
			playerWeaponComp.LVL2AttackCooldown = lootWeaponComp.LVL2AttackCooldown;
			playerWeaponComp.LVL3AttackCooldown = lootWeaponComp.LVL3AttackCooldown;
			playerWeaponComp.Lifetime = lootWeaponComp.Lifetime;
			playerWeaponComp.AttackHitboxScale = lootWeaponComp.AttackHitboxScale;
			playerWeaponComp.FireCriticalHitChance = lootWeaponComp.FireCriticalHitChance;
			playerWeaponComp.EarthDamage = lootWeaponComp.EarthDamage;
			playerWeaponComp.WindSpeed = lootWeaponComp.WindSpeed;
			playerWeaponComp.WaterHealing = lootWeaponComp.WaterHealing;
			playerWeaponComp.IsFullyUpgraded = lootWeaponComp.IsFullyUpgraded;
			playerWeaponComp.ProjectileSpeed = lootWeaponComp.ProjectileSpeed;

			lootWeaponComp.Type = type;
			lootWeaponComp.Level = level;
			lootWeaponComp.Speciality = speciality;
			lootWeaponComp.MaxAttackRange = maxAttackRange;
			lootWeaponComp.MinAttackRange = minAttackRange;
			lootWeaponComp.Damage;
			lootWeaponComp.CriticalHit;
			lootWeaponComp.CriticalHitChance = lootWeaponComp.CriticalHitChance;
			lootWeaponComp.LVL1AttackCooldown = lvl1Cooldown;
			lootWeaponComp.LVL2AttackCooldown = lvl2Cooldown;
			lootWeaponComp.LVL3AttackCooldown = lvl3Cooldown;
			lootWeaponComp.Lifetime = lifetime;
			lootWeaponComp.AttackHitboxScale = attackHitboxScale;
			lootWeaponComp.FireCriticalHitChance = fire;
			lootWeaponComp.EarthDamage = earth;
			lootWeaponComp.WindSpeed = wind;
			lootWeaponComp.WaterHealing = water;
			lootWeaponComp.IsFullyUpgraded = upgraded;
			lootWeaponComp.ProjectileSpeed = projectileSpeed;


			//Frosty::ECS::CWeapon tempWeapon;
			//tempWeapon = m_World->GetComponent<Frosty::ECS::CWeapon>(playerWeapon);
			//m_World->GetComponent<Frosty::ECS::CWeapon>(playerWeapon) = m_World->GetComponent<Frosty::ECS::CWeapon>(lootWeapon);
			//m_World->GetComponent<Frosty::ECS::CWeapon>(lootWeapon) = tempWeapon;



			// Only switch CMesh and CMaterial when weapon stats have been swapped
			SwapMesh(playerWeapon, lootWeapon);
			SwapMaterial(playerWeapon, lootWeapon);
			Frosty::Renderer::SwapEntity(playerWeapon, lootWeapon);
		}
	}

	void PlayerControllerSystem::SwapMesh(const std::shared_ptr<Frosty::ECS::Entity>& playerWeapon, const std::shared_ptr<Frosty::ECS::Entity>& lootWeapon)
	{
		// Swap CMesh
		if ((m_World->HasComponent<Frosty::ECS::CMesh>(playerWeapon)) && (m_World->HasComponent<Frosty::ECS::CMesh>(lootWeapon)))
		{
			if ((m_World->GetComponent<Frosty::ECS::CMesh>(playerWeapon)) != (m_World->GetComponent<Frosty::ECS::CMesh>(lootWeapon)))
			{
				//Transform must be assigned since parentMatrix(Player) being applied to is the only thing keeping the held weapon from origo.
				Frosty::ECS::CTransform tempTransform;
				tempTransform = m_World->GetComponent<Frosty::ECS::CTransform>(lootWeapon);
				auto& playerWeaponComp = m_World->GetComponent<Frosty::ECS::CMesh>(playerWeapon);
				auto& lootWeaponComp = m_World->GetComponent<Frosty::ECS::CMesh>(lootWeapon);

				std::shared_ptr<Frosty::VertexArray> mesh = playerWeaponComp.Mesh;
				bool renderMesh = playerWeaponComp.RenderMesh;
				//glm::mat4* parentMatrix = playerWeaponComp.parentMatrix;
				//glm::mat4* animOffset = playerWeaponComp.animOffset;

				playerWeaponComp.Mesh = lootWeaponComp.Mesh;
				playerWeaponComp.RenderMesh = lootWeaponComp.RenderMesh;

				lootWeaponComp.Mesh = mesh;
				lootWeaponComp.RenderMesh = renderMesh;

				//Frosty::ECS::CMesh tempMesh;
				//tempMesh = m_World->GetComponent<Frosty::ECS::CMesh>(playerWeapon);
				//tempMesh.parentMatrix = m_World->GetComponent<Frosty::ECS::CMesh>(playerWeapon).parentMatrix;
				//tempMesh.animOffset = m_World->GetComponent<Frosty::ECS::CMesh>(playerWeapon).animOffset;
				//m_World->GetComponent<Frosty::ECS::CMesh>(playerWeapon) = m_World->GetComponent<Frosty::ECS::CMesh>(lootWeapon);
				//
				////Make sure it actually ends up in the hand. Default assignment does not do this properly.
				//m_World->GetComponent<Frosty::ECS::CMesh>(playerWeapon).parentMatrix = tempMesh.parentMatrix;
				//m_World->GetComponent<Frosty::ECS::CMesh>(playerWeapon).animOffset = tempMesh.animOffset;

				//Reset it otherwise the opposite object will still be parented.
				//tempMesh.parentMatrix = nullptr;
				//tempMesh.animOffset = nullptr;
				//m_World->GetComponent<Frosty::ECS::CMesh>(lootWeapon) = tempMesh;

				//m_World->GetComponent<Frosty::ECS::CTransform>(lootWeapon) = tempTransform;
			}
		}
	}

	void PlayerControllerSystem::SwapMaterial(const std::shared_ptr<Frosty::ECS::Entity>& playerWeapon, const std::shared_ptr<Frosty::ECS::Entity>& lootWeapon)
	{
		// Swap CMesh
		if ((m_World->HasComponent<Frosty::ECS::CMaterial>(playerWeapon)) && (m_World->HasComponent<Frosty::ECS::CMaterial>(lootWeapon)))
		{
			if ((m_World->GetComponent<Frosty::ECS::CMaterial>(playerWeapon)) != (m_World->GetComponent<Frosty::ECS::CMaterial>(lootWeapon)))
			{
				auto& playerWeaponComp = m_World->GetComponent<Frosty::ECS::CMaterial>(playerWeapon);
				auto& lootWeaponComp = m_World->GetComponent<Frosty::ECS::CMaterial>(lootWeapon);

				std::shared_ptr<Frosty::Shader>& shader = playerWeaponComp.UseShader;
				glm::vec4 albedo = playerWeaponComp.Albedo;
				std::shared_ptr<Frosty::Texture2D> diffuseTex = playerWeaponComp.DiffuseTexture;
				std::shared_ptr<Frosty::Texture2D> specularTex = playerWeaponComp.SpecularTexture;
				std::shared_ptr<Frosty::Texture2D> normalTex = playerWeaponComp.NormalTexture;
				std::shared_ptr<Frosty::Texture2D> blendMapTex = playerWeaponComp.BlendMapTexture;
				std::shared_ptr<Frosty::Texture2D> blendTex1 = playerWeaponComp.BlendTexture1;
				std::shared_ptr<Frosty::Texture2D> blendTex2 = playerWeaponComp.BlendTexture2;
				float specStrength = playerWeaponComp.SpecularStrength;
				int shininess = playerWeaponComp.Shininess;
				glm::vec2 texScale = playerWeaponComp.TextureScale;
				bool transparency = playerWeaponComp.HasTransparency;

				playerWeaponComp.UseShader = lootWeaponComp.UseShader;
				playerWeaponComp.Albedo = lootWeaponComp.Albedo;
				playerWeaponComp.DiffuseTexture = lootWeaponComp.DiffuseTexture;
				playerWeaponComp.SpecularTexture = lootWeaponComp.SpecularTexture;
				playerWeaponComp.NormalTexture = lootWeaponComp.NormalTexture;
				playerWeaponComp.BlendMapTexture = lootWeaponComp.BlendMapTexture;
				playerWeaponComp.BlendTexture1 = lootWeaponComp.BlendTexture1;
				playerWeaponComp.BlendTexture2 = lootWeaponComp.BlendTexture2;
				playerWeaponComp.SpecularStrength = lootWeaponComp.SpecularStrength;
				playerWeaponComp.Shininess = lootWeaponComp.Shininess;
				playerWeaponComp.TextureScale = lootWeaponComp.TextureScale;
				playerWeaponComp.HasTransparency = lootWeaponComp.HasTransparency;

				lootWeaponComp.UseShader = shader;
				lootWeaponComp.Albedo = albedo;
				lootWeaponComp.DiffuseTexture = diffuseTex;
				lootWeaponComp.SpecularTexture = specularTex;
				lootWeaponComp.NormalTexture = normalTex;
				lootWeaponComp.BlendMapTexture = blendMapTex;
				lootWeaponComp.BlendTexture1 = blendTex1;
				lootWeaponComp.BlendTexture2 = blendTex2;
				lootWeaponComp.SpecularStrength = specStrength;
				lootWeaponComp.Shininess = shininess;
				lootWeaponComp.TextureScale = texScale;
				lootWeaponComp.HasTransparency = transparency;


				//Frosty::ECS::CMaterial tempMaterial;
				//tempMaterial = m_World->GetComponent<Frosty::ECS::CMaterial>(playerWeapon);
				//m_World->GetComponent<Frosty::ECS::CMaterial>(playerWeapon) = m_World->GetComponent<Frosty::ECS::CMaterial>(lootWeapon);
				//m_World->GetComponent<Frosty::ECS::CMaterial>(lootWeapon) = tempMaterial;
			}
		}
	}

	void PlayerControllerSystem::SwapLootType(const std::shared_ptr<Frosty::ECS::Entity>& playerWeapon, const std::shared_ptr<Frosty::ECS::Entity>& lootWeapon)
	{
		auto& playerWeaponComp = m_World->GetComponent<Frosty::ECS::CWeapon>(playerWeapon);
		auto& lootComp = m_World->GetComponent<Frosty::ECS::CLootable>(lootWeapon);
		int level = playerWeaponComp.Level;

		if (playerWeaponComp.Type == Frosty::ECS::CWeapon::WeaponType::Sword)
		{
			if (level == 1)
				lootComp.Weapon = Frosty::ECS::CLootable::WeaponType::Sword1;
			else if (level == 2)
				lootComp.Weapon = Frosty::ECS::CLootable::WeaponType::Sword2;
			else if (level == 3)
				lootComp.Weapon = Frosty::ECS::CLootable::WeaponType::Sword3;

		}
		else if (playerWeaponComp.Type == Frosty::ECS::CWeapon::WeaponType::Bow)
		{
			if (level == 1)
				lootComp.Weapon = Frosty::ECS::CLootable::WeaponType::Bow1;
			else if (level == 2)
				lootComp.Weapon = Frosty::ECS::CLootable::WeaponType::Bow2;
			else if (level == 3)
				lootComp.Weapon = Frosty::ECS::CLootable::WeaponType::Bow3;
		}
	}

	void PlayerControllerSystem::UpdateHUD(size_t index)
	{
		if (m_World->HasComponent<Frosty::ECS::CGUI>(m_Transform[index]->EntityPtr))
		{
			auto& window = Frosty::Application::Get().GetWindow();

			//m_HeightMultiplier = window.GetHeightMultiplier();
			//m_WidthMultiplier = window.GetWidthMultiplier();

			m_HeightMultiplier = 1;
			m_WidthMultiplier = 1;

			auto& HUD = m_World->GetComponent<Frosty::ECS::CGUI>(m_Transform[index]->EntityPtr);

			if (Frosty::InputManager::IsKeyPressed(FY_KEY_P))
			{
				/*auto& weaponComp = (m_World->GetComponent<Frosty::ECS::CWeapon>(m_Player[index]->Weapon->EntityPtr));
				weaponComp.FireCriticalHitChance += 0.1f;
				HUD.Layout.sprites.at(6).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				HUD.Layout.sprites.at(6).SetTranslateSprite(glm::vec2(960 / 1.5f, (540 / 1.5f) + 200));
				HUD.Layout.sprites.at(6).SetScaleSprite(glm::vec2(1.5, 1.5));
				m_Player[index]->ElementDisplayTimer = Frosty::Time::CurrentTime();

				SetPickUpText(index, "tjena");*/

				//Frosty::EventBus::GetEventBus()->Publish<Frosty::UpgradeWeaponEvent>(Frosty::UpgradeWeaponEvent());
			}



			//Items
			HUD.Layout.texts[0].SetText(std::string(std::to_string(m_Inventory[index]->CurrentHealingPotions) + "/" + std::string(std::to_string(m_Inventory[index]->MaxHealingPotions))));
			HUD.Layout.texts[1].SetText(std::string(std::to_string(m_Inventory[index]->CurrentSpeedPotions) + "/" + std::string(std::to_string(m_Inventory[index]->MaxSpeedBoots))));
			HUD.Layout.texts[2].SetText(std::string(std::to_string(m_Inventory[index]->CurrentBaitAmount) + "/" + std::string(std::to_string(m_Inventory[index]->MaxBaitAmount))));
			HUD.Layout.texts[3].SetText(std::string(std::to_string(m_Inventory[index]->CurrentWolfsbane)));

			if (m_Inventory[index]->CurrentHealingPotions == 0)
			{
				HUD.Layout.sprites[10].SetColorSprite(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
				HUD.Layout.texts[18].SetText(std::string(""));
				HUD.Layout.texts[0].SetColor(glm::vec3(0.75f, 0.75f, 0.50f));
			}
			else
			{
				HUD.Layout.sprites[10].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				HUD.Layout.texts[18].SetText(std::string("[1]"));
				HUD.Layout.texts[0].SetColor(glm::vec3(1.0f, 1.0f, 0.75f));

			}

			if (m_Inventory[index]->CurrentSpeedPotions == 0)
			{
				HUD.Layout.sprites[11].SetColorSprite(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
				HUD.Layout.texts[19].SetText(std::string(""));
				HUD.Layout.texts[1].SetColor(glm::vec3(0.75f, 0.75f, 0.50f));

			}
			else
			{
				HUD.Layout.sprites[11].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				HUD.Layout.texts[19].SetText(std::string("[2]"));
				HUD.Layout.texts[1].SetColor(glm::vec3(1.0f, 1.0f, 0.75f));

			}

			if (m_Inventory[index]->CurrentBaitAmount == 0)
			{
				HUD.Layout.sprites[12].SetColorSprite(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
				HUD.Layout.texts[20].SetText(std::string(""));
				HUD.Layout.texts[2].SetColor(glm::vec3(0.5f, 0.5f, 0.25f));

			}
			else
			{
				HUD.Layout.sprites[12].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				HUD.Layout.texts[20].SetText(std::string("[Q]"));
				HUD.Layout.texts[2].SetColor(glm::vec3(1.0f, 1.0f, 0.75f));

			}

			//Points
			HUD.Layout.texts[4].SetText(std::string("Points: " + std::to_string(m_Player[index]->Score)));

			//Temp Health
			//HUD.Layout.texts[5].SetText(std::string(std::to_string((int)m_Health[index]->CurrentHealth) + "/" + std::to_string((int)m_Health[index]->MaxHealth)));

			int currentHealth = 0;

			if (m_Health[index]->CurrentHealth > 0)
			{
				currentHealth = m_Health[index]->CurrentHealth;
			}


			int nrOfFilledHearts = currentHealth / 4;
			int nrOfHeartQuadrants = currentHealth % 4;

			int healthSpriteID = 20;
			for (int i = 0; i < nrOfFilledHearts && nrOfFilledHearts <= m_Health[index]->MaxHealth; i++)
			{
				if (healthSpriteID < 30)
				{
					HUD.Layout.sprites[healthSpriteID].SetImage("Heart_4");
					HUD.Layout.sprites[healthSpriteID].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					healthSpriteID++;
				}
			}

			if (nrOfHeartQuadrants == 1)
			{
				if (healthSpriteID < 30)
				{
					HUD.Layout.sprites[healthSpriteID].SetImage("Heart_1");
					HUD.Layout.sprites[healthSpriteID].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}
			else if (nrOfHeartQuadrants == 2)
			{
				if (healthSpriteID < 30)
				{
					HUD.Layout.sprites[healthSpriteID].SetImage("Heart_2");
					HUD.Layout.sprites[healthSpriteID].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}
			else if (nrOfHeartQuadrants == 3)
			{
				if (healthSpriteID < 30)
				{
					HUD.Layout.sprites[healthSpriteID].SetImage("Heart_3");
					HUD.Layout.sprites[healthSpriteID].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}
			else if (m_Health[index]->CurrentHealth < m_Health[index]->MaxHealth)
			{
				if (healthSpriteID < 30)
				{
					HUD.Layout.sprites[healthSpriteID].SetImage("Heart_0");
					HUD.Layout.sprites[healthSpriteID].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}

			int nrOfEmptyHearts = (m_Health[index]->MaxHealth - currentHealth) / 4;
			healthSpriteID = 20 + currentHealth / 4;

			if (nrOfHeartQuadrants > 0)
			{
				healthSpriteID++;
			}

			for (int i = 0; i < nrOfEmptyHearts; i++)
			{
				if (healthSpriteID < 30)
				{
					HUD.Layout.sprites[healthSpriteID].SetImage("Heart_0");
					HUD.Layout.sprites[healthSpriteID].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					healthSpriteID++;
				}
			}

			//Pickup Text
			if (Frosty::Time::CurrentTime() - m_Player[index]->PickUpTextTimer >= m_Player[index]->PickUpTextTime)
			{
				HUD.Layout.texts[6].SetText("");
			}

			//Attack cooldown
			auto& weapon = m_Player[index]->Weapon;
			if (weapon->Level == 3)
			{
				HUD.Layout.texts[14].SetText(std::string("[SPACE]"));

				float timer = weapon->LVL3AttackCooldownTimer;
				float time = Frosty::Time::CurrentTime();
				float dif = (time - timer);
				float cooldown = weapon->LVL3AttackCooldown - dif;
				if (cooldown > 0)
				{
					int cooldown1 = (int)cooldown;
					int cooldown2 = (int)((cooldown - cooldown1) * 10);
					HUD.Layout.texts[7].SetText(std::string(std::to_string(cooldown1) + "." + std::to_string(cooldown2)));
					HUD.Layout.sprites[5].SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.90f));
				}
				else
				{
					HUD.Layout.texts[7].SetText(std::string(""));
					HUD.Layout.sprites[5].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

				}
			}
			else
			{
				HUD.Layout.texts[14].SetText(std::string(""));
				HUD.Layout.sprites[5].SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));

			}

			if (weapon->Level >= 2)
			{
				HUD.Layout.texts[15].SetText(std::string("[R-MOUSE]"));

				float timer = weapon->LVL2AttackCooldownTimer;
				float time = Frosty::Time::CurrentTime();
				float dif = (time - timer);
				float cooldown = weapon->LVL2AttackCooldown - dif;
				if (cooldown > 0)
				{
					int cooldown1 = (int)cooldown;
					int cooldown2 = (int)((cooldown - cooldown1) * 10);
					HUD.Layout.texts[8].SetText(std::string(std::to_string(cooldown1) + "." + std::to_string(cooldown2)));
					HUD.Layout.sprites[4].SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.90f));


				}
				else
				{
					HUD.Layout.texts[8].SetText(std::string(""));
					HUD.Layout.sprites[4].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

				}
			}
			else
			{
				HUD.Layout.texts[15].SetText(std::string(""));
				HUD.Layout.sprites[4].SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));

			}

			if (weapon->Level >= 1)
			{
				HUD.Layout.texts[16].SetText(std::string("[L-MOUSE]"));

				float timer = weapon->LVL1AttackCooldownTimer;
				float time = Frosty::Time::CurrentTime();
				float dif = (time - timer);
				float cooldown = weapon->LVL1AttackCooldown - dif;
				if (cooldown > 0)
				{
					int cooldown1 = (int)cooldown;
					int cooldown2 = (int)((cooldown - cooldown1) * 10);
					HUD.Layout.texts[9].SetText(std::string(std::to_string(cooldown1) + "." + std::to_string(cooldown2)));
					HUD.Layout.sprites[3].SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.90f));


				}
				else
				{
					HUD.Layout.texts[9].SetText(std::string(""));
					HUD.Layout.sprites[3].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

				}
			}
			else
			{
				HUD.Layout.texts[16].SetText(std::string(""));
				HUD.Layout.sprites[3].SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));

			}


			//Dash cooldown
			if (m_Dash[index]->CurrentCooldown > 0)
			{
				int cooldown1 = (int)(m_Dash[index]->CurrentCooldown);
				int cooldown2 = (int)(((m_Dash[index]->CurrentCooldown - cooldown1) * 10));

				HUD.Layout.texts[10].SetText(std::string(std::to_string(cooldown1) + "." + std::to_string(cooldown2)));
				HUD.Layout.sprites[14].SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.90f));

			}
			else
			{
				HUD.Layout.texts[10].SetText(std::string(""));
				HUD.Layout.sprites[14].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

			}


			m_Inventory[index]->HealingTimer;

			//Item Cooldowns
			float CurrentTime = Frosty::Time::CurrentTime();

			//Health
			float timer = m_Inventory[index]->HealingTimer;
			float dif = (CurrentTime - timer);
			float cooldown = m_Inventory[index]->HealingCooldown - dif;

			if (cooldown > 0)
			{
				int cooldown1 = (int)cooldown;
				int cooldown2 = (int)((cooldown - cooldown1) * 10);
				HUD.Layout.texts[11].SetText(std::string(std::to_string(cooldown1) + "." + std::to_string(cooldown2)));
				HUD.Layout.sprites[10].SetColorSprite(glm::vec4(0.2f, 0.2f, 0.2f, 0.75f));

			}
			else
			{
				HUD.Layout.texts[11].SetText(std::string(""));
				if (m_Inventory[index]->CurrentHealingPotions != 0)
				{
					HUD.Layout.sprites[10].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				}

			}

			//Speed
			timer = m_Inventory[index]->SpeedTimer;
			dif = (CurrentTime - timer);
			cooldown = m_Inventory[index]->SpeedCooldown - dif;

			if (cooldown > 0)
			{
				int cooldown1 = (int)cooldown;
				int cooldown2 = (int)((cooldown - cooldown1) * 10);
				HUD.Layout.texts[12].SetText(std::string(std::to_string(cooldown1) + "." + std::to_string(cooldown2)));
				HUD.Layout.sprites[11].SetColorSprite(glm::vec4(0.2f, 0.2f, 0.2f, 0.75f));

			}
			else
			{
				HUD.Layout.texts[12].SetText(std::string(""));
				if (m_Inventory[index]->CurrentSpeedPotions != 0)
				{
					HUD.Layout.sprites[11].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				}

			}

			//Bait
			timer = m_Inventory[index]->BaitTimer;
			dif = (CurrentTime - timer);
			cooldown = m_Inventory[index]->BaitCooldown - dif;

			if (cooldown > 0)
			{
				int cooldown1 = (int)cooldown;
				int cooldown2 = (int)((cooldown - cooldown1) * 10);
				HUD.Layout.texts[13].SetText(std::string(std::to_string(cooldown1) + "." + std::to_string(cooldown2)));
				HUD.Layout.sprites[12].SetColorSprite(glm::vec4(0.2f, 0.2f, 0.2f, 0.75f));

			}
			else
			{
				HUD.Layout.texts[13].SetText(std::string(""));
				if (m_Inventory[index]->CurrentBaitAmount != 0)
				{
					HUD.Layout.sprites[12].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				}

			}

			//Speed boots
			int bootSpriteID = 15;
			for (int i = 0; i < m_Inventory[index]->CurrentSpeedBoots && m_Inventory[index]->CurrentSpeedBoots <= 5; i++)
			{

				HUD.Layout.sprites[bootSpriteID].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				bootSpriteID++;
			}

			//Elemental

			if (Frosty::Time::CurrentTime() - m_Player[index]->ElementDisplayTimer < m_Player[index]->ElementDisplayTime)
			{
				m_Player[index]->ElementMoveTimer = Frosty::Time::CurrentTime();

			}

			//Earth
			if (m_Player[index]->Weapon->EarthDamage > 0.0f)
			{

				float timeLeft = Frosty::Time::CurrentTime() - m_Player[index]->ElementMoveTimer;

				if (timeLeft <= m_Player[index]->ElementMoveTime && timeLeft >= 0 && !m_Player[index]->Weapon->HasDoneEarthSprite)
				{
					float percentage = (Frosty::Time::CurrentTime() - m_Player[index]->ElementMoveTimer) / m_Player[index]->ElementMoveTime;


					glm::vec2 posA{ m_WindowWidth / 2, (m_WindowHeight / 2) + 150 * m_HeightMultiplier };
					glm::vec2 posB{ 30 * m_WidthMultiplier,30 * m_HeightMultiplier };
					glm::vec2 posDif = posA - posB;

					glm::vec2 scaleA{ 1.5 * m_WidthMultiplier, 1.5 * m_HeightMultiplier };
					glm::vec2 scaleB{ 0.5 * m_WidthMultiplier, 0.5 * m_HeightMultiplier };
					glm::vec2 scaleDif = scaleA - scaleB;

					HUD.Layout.sprites[6].SetTranslateSprite(posA - posDif * percentage);
					HUD.Layout.sprites[6].SetScaleSprite(scaleA - scaleDif * percentage);


				}
				else
				{
					HUD.Layout.sprites[6].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					HUD.Layout.sprites[6].SetTranslateSprite(glm::vec2(30 * m_WidthMultiplier, 30 * m_HeightMultiplier));
					HUD.Layout.sprites[6].SetScaleSprite(glm::vec2(0.5 * m_WidthMultiplier, 0.5 * m_HeightMultiplier));
					m_Player[index]->Weapon->HasDoneEarthSprite = true;
				}

			}
			else
			{
				HUD.Layout.sprites[6].SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));
			}

			//Fire
			if (m_Player[index]->Weapon->FireCriticalHitChance > 0.0f)
			{
				float timeLeft = Frosty::Time::CurrentTime() - m_Player[index]->ElementMoveTimer;
				if (timeLeft <= m_Player[index]->ElementMoveTime && timeLeft >= 0 && !m_Player[index]->Weapon->HasDoneFireSprite)
				{
					float percentage = (Frosty::Time::CurrentTime() - m_Player[index]->ElementMoveTimer) / m_Player[index]->ElementMoveTime;


					glm::vec2 posA{ m_WindowWidth / 2, (m_WindowHeight / 2) + 150 * m_HeightMultiplier };
					glm::vec2 posB{ 50 * m_WidthMultiplier,30 * m_HeightMultiplier };
					glm::vec2 posDif = posA - posB;

					glm::vec2 scaleA{ 1.5 * m_WidthMultiplier, 1.5 * m_HeightMultiplier };
					glm::vec2 scaleB{ 0.5 * m_WidthMultiplier, 0.5 * m_HeightMultiplier };
					glm::vec2 scaleDif = scaleA - scaleB;

					HUD.Layout.sprites[7].SetTranslateSprite(posA - posDif * percentage);
					HUD.Layout.sprites[7].SetScaleSprite(scaleA - scaleDif * percentage);


				}
				else
				{
					HUD.Layout.sprites[7].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					HUD.Layout.sprites[7].SetTranslateSprite(glm::vec2(50 * m_WidthMultiplier, 30 * m_HeightMultiplier));
					HUD.Layout.sprites[7].SetScaleSprite(glm::vec2(0.5 * m_WidthMultiplier, 0.5 * m_HeightMultiplier));
					m_Player[index]->Weapon->HasDoneFireSprite = true;
				}
			}
			else
			{
				HUD.Layout.sprites[7].SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));
			}
			//Water
			if (m_Player[index]->Weapon->WaterHealing > 0.0f)
			{
				float timeLeft = Frosty::Time::CurrentTime() - m_Player[index]->ElementMoveTimer;
				if (timeLeft <= m_Player[index]->ElementMoveTime && timeLeft >= 0 && !m_Player[index]->Weapon->HasDoneWaterSprite)
				{
					float percentage = (Frosty::Time::CurrentTime() - m_Player[index]->ElementMoveTimer) / m_Player[index]->ElementMoveTime;


					glm::vec2 posA{ m_WindowWidth / 2, (m_WindowHeight / 2) + 150 * m_HeightMultiplier };
					glm::vec2 posB{ 70 * m_WidthMultiplier,30 * m_HeightMultiplier };
					glm::vec2 posDif = posA - posB;

					glm::vec2 scaleA{ 1.5 * m_WidthMultiplier, 1.5 * m_HeightMultiplier };
					glm::vec2 scaleB{ 0.5 * m_WidthMultiplier, 0.5 * m_HeightMultiplier };
					glm::vec2 scaleDif = scaleA - scaleB;

					HUD.Layout.sprites[8].SetTranslateSprite(posA - posDif * percentage);
					HUD.Layout.sprites[8].SetScaleSprite(scaleA - scaleDif * percentage);


				}
				else
				{
					HUD.Layout.sprites[8].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					HUD.Layout.sprites[8].SetTranslateSprite(glm::vec2(70 * m_WidthMultiplier, 30 * m_HeightMultiplier));
					HUD.Layout.sprites[8].SetScaleSprite(glm::vec2(0.5 * m_WidthMultiplier, 0.5 * m_HeightMultiplier));
					m_Player[index]->Weapon->HasDoneWaterSprite = true;
				}
			}
			else
			{
				HUD.Layout.sprites[8].SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));
			}
			//Wind
			if (m_Player[index]->Weapon->WindSpeed > 0.0f)
			{
				float timeLeft = Frosty::Time::CurrentTime() - m_Player[index]->ElementMoveTimer;
				if (timeLeft <= m_Player[index]->ElementMoveTime && timeLeft >= 0 && !m_Player[index]->Weapon->HasDoneWindSprite)
				{
					float percentage = (Frosty::Time::CurrentTime() - m_Player[index]->ElementMoveTimer) / m_Player[index]->ElementMoveTime;


					glm::vec2 posA{ m_WindowWidth / 2, (m_WindowHeight / 2) + 150 * m_HeightMultiplier };
					glm::vec2 posB{ 90 * m_WidthMultiplier,30 * m_HeightMultiplier };
					glm::vec2 posDif = posA - posB;

					glm::vec2 scaleA{ 1.5 * m_WidthMultiplier, 1.5 * m_HeightMultiplier };
					glm::vec2 scaleB{ 0.5 * m_WidthMultiplier, 0.5 * m_HeightMultiplier };
					glm::vec2 scaleDif = scaleA - scaleB;

					HUD.Layout.sprites[9].SetTranslateSprite(posA - posDif * percentage);
					HUD.Layout.sprites[9].SetScaleSprite(scaleA - scaleDif * percentage);


				}
				else
				{
					HUD.Layout.sprites[9].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					HUD.Layout.sprites[9].SetTranslateSprite(glm::vec2(90 * m_WidthMultiplier, 30 * m_HeightMultiplier));
					HUD.Layout.sprites[9].SetScaleSprite(glm::vec2(0.5 * m_WidthMultiplier, 0.5 * m_HeightMultiplier));
					m_Player[index]->Weapon->HasDoneWindSprite = true;
				}
			}
			else
			{
				HUD.Layout.sprites[9].SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));
			}

			//Damage Effect
			float timeLeft = Frosty::Time::CurrentTime() - m_Player[index]->DamageEffectTimer;
			if (timeLeft <= m_Player[index]->DamageEffectTime && timeLeft >= 0)
			{
				float percentage = m_Player[index]->ElementMoveTime / (Frosty::Time::CurrentTime() - m_Player[index]->ElementMoveTimer);
				percentage /= 100;

				if (m_Health[index]->CurrentHealth <= 4 && percentage <= 0.75)
				{
					percentage = 0.75;
				}
				HUD.Layout.sprites[0].SetColorSprite(glm::vec4(1.0f * percentage, 0.0f, 0.0f, 0.75f));


			}
			else if (m_Health[index]->CurrentHealth <= 4)
			{
				HUD.Layout.sprites[0].SetColorSprite(glm::vec4(0.75f, 0.0f, 0.0f, 0.75f));
			}
			else
			{
				HUD.Layout.sprites[0].SetColorSprite(glm::vec4(0.0f, 0.0f, 0.0f, 0.75f));
			}


			//BossFearEffect
			timeLeft = Frosty::Time::CurrentTime() - m_Player[index]->BossFearEffectTimer;
			if (timeLeft <= m_Player[index]->BossFearEffectTime && timeLeft >= 0)
			{
				float percentage = (Frosty::Time::CurrentTime() - m_Player[index]->BossFearEffectTimer) / m_Player[index]->BossFearEffectTime;
				HUD.Layout.sprites[1].SetColorSprite(glm::vec4(1.0f , 1.0f, 1.0f, 1.0f - 1.0f* percentage));
				//FY_INFO("OnBossFearEffect reset");
			}
		}
	}

	void PlayerControllerSystem::SetPickUpText(size_t index, std::string text)
	{


		float posX = (m_WindowWidth / 2) - (text.size() / 2) * 11 * m_WidthMultiplier;

		if (m_World->HasComponent<Frosty::ECS::CGUI>(m_Transform[index]->EntityPtr))
		{
			auto& HUD = m_World->GetComponent<Frosty::ECS::CGUI>(m_Transform[index]->EntityPtr);

			HUD.Layout.texts[6].SetText(text);
			HUD.Layout.texts[6].SetScale(glm::vec2(0.75f * m_WidthMultiplier, 0.75f * m_HeightMultiplier));
			HUD.Layout.texts[6].SetPosition(glm::vec2(posX, 425 * m_HeightMultiplier));

			m_Player[index]->PickUpTextTimer = Frosty::Time::CurrentTime();
		}
	}

	void PlayerControllerSystem::ResetAllHUDWeaponInfo(size_t index)
	{
		auto& HUD = m_World->GetComponent<Frosty::ECS::CGUI>(m_Transform[index]->EntityPtr);
		HUD.Layout.texts[7].SetText(std::string(""));
		HUD.Layout.texts[8].SetText(std::string(""));
		HUD.Layout.texts[9].SetText(std::string(""));
	}

	void PlayerControllerSystem::OnDamage()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			auto& HUD = m_World->GetComponent<Frosty::ECS::CGUI>(m_Transform[i]->EntityPtr);
			HUD.Layout.sprites[0].SetColorSprite(glm::vec4(1.0f, 0.0f, 0.0f, 0.75f));

			m_Player[i]->DamageEffectTimer = Frosty::Time::CurrentTime();
		}

	}

	void PlayerControllerSystem::OnBossFearEffect(Frosty::BossFearEffectEvent& e)
	{
		FY_INFO("OnBossFearEffect");
		glm::vec2 pos;
		float width = 1280.f;
		float height = 720.f;

		if (e.GetDirectionToBoss().x > 0)
		{
			pos.x = width; //Right
		}
		else if (e.GetDirectionToBoss().x < 0)
		{
			pos.x = 0; //Left
		}
		else
		{
			pos.x = width/2; //Midle
		}

		if (e.GetDirectionToBoss().y > 0)
		{
			pos.y = 0; //Bottom
		}
		else if (e.GetDirectionToBoss().y < 0)
		{
			pos.y = height; //Top

		}
		else
		{
			pos.y = height / 2; //Midle
		}


		for (size_t i = 1; i < p_Total; i++)
		{
			auto& HUD = m_World->GetComponent<Frosty::ECS::CGUI>(m_Transform[i]->EntityPtr);
			HUD.Layout.sprites[1].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			HUD.Layout.sprites[1].SetTranslateSprite(pos);
			HUD.Layout.sprites[1].SetScaleSprite(glm::vec2(10,10));
			
			m_Player[i]->BossFearEffectTimer = Frosty::Time::CurrentTime();
		}
	}


}