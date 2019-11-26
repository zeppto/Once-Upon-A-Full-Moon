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
	}

	void PlayerControllerSystem::OnUpdate()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Dash[i]->CurrentCooldown > 0.0f) m_Dash[i]->CurrentCooldown -= Frosty::Time::DeltaTime();
			if (Frosty::Time::CurrentTime() - m_Inventory[i]->SpeedTimer >= m_Inventory[i]->SpeedCooldown && m_Physics[i]->SpeedMultiplier > 1.0f)
			{
				m_Physics[i]->SpeedMultiplier = 1.0f;
			}

			if (m_Dash[i]->Active)
			{
				m_Dash[i]->DistanceDashed += glm::length(m_Physics[i]->Direction * m_Physics[i]->Speed * m_Physics[i]->SpeedMultiplier * Frosty::Time::DeltaTime());
				if (m_Dash[i]->DistanceDashed >= m_Dash[i]->DISTANCE / 1000.0f)
				{
					m_Dash[i]->Active = false;
					m_Dash[i]->DistanceDashed = 0.0f;
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

			if (Frosty::InputManager::IsKeyPressed(m_Player[index]->DashKey))
			{
				// Check if entity has CDash component before publishing
				if (m_Dash[index]->CurrentCooldown <= 0.0f)
				{
					m_Dash[index]->Active = true;
					Frosty::EventBus::GetEventBus()->Publish<Frosty::DashEvent>(Frosty::DashEvent(m_Player[index]->EntityPtr));
					//m_Physics[index]->Velocity *= m_Dash[index]->SpeedMultiplier;
					m_Physics[index]->SpeedMultiplier = m_Dash[index]->SpeedMultiplier;
					m_Dash[index]->CurrentCooldown = m_Dash[index]->COOLDOWN / 1000.0f;
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

		if (Frosty::Time::CurrentTime() - weaponComp.LVL1AttackCooldownTimer >= weaponComp.LVL1AttackCooldown - weaponComp.WindSpeed)
		{
			switch (weaponComp.Type)
			{
			case Frosty::ECS::CWeapon::WeaponType::Sword:
				Frosty::EventBus::GetEventBus()->Publish <Frosty::PlayAnimEvent>(Frosty::PlayAnimEvent(weaponCarrier, 1));
				CreateLVL1BoundingBox(weaponCarrier, weaponComp.EntityPtr);
				break;
			case Frosty::ECS::CWeapon::WeaponType::Bow:
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

		if (Frosty::Time::CurrentTime() - weaponComp.LVL2AttackCooldownTimer >= weaponComp.LVL2AttackCooldown - weaponComp.WindSpeed)
		{
			switch (weaponComp.Type)
			{
			case Frosty::ECS::CWeapon::WeaponType::Sword:
				Frosty::EventBus::GetEventBus()->Publish <Frosty::PlayAnimEvent>(Frosty::PlayAnimEvent(weaponCarrier, 2));
				CreateLVL2BoundingBox(weaponCarrier, weaponComp.EntityPtr);
				break;
			case Frosty::ECS::CWeapon::WeaponType::Bow:
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

		if (Frosty::Time::CurrentTime() - weaponComp.LVL3AttackCooldownTimer >= weaponComp.LVL3AttackCooldown - weaponComp.WindSpeed)
		{
			switch (weaponComp.Type)
			{
			case Frosty::ECS::CWeapon::WeaponType::Sword:
				Frosty::EventBus::GetEventBus()->Publish <Frosty::PlayAnimEvent>(Frosty::PlayAnimEvent(weaponCarrier, 3));
				CreateLVL3BoundingBox(weaponCarrier, weaponComp.EntityPtr);
				break;
			case Frosty::ECS::CWeapon::WeaponType::Bow:
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
		glm::vec3 spawnScale = glm::vec3(10.0f, 6.0f, 4.0f);
		auto& sword = m_World->CreateEntity({ spawnPos.x, 3.0f, spawnPos.z }, attackerTransform.Rotation, spawnScale);

		m_World->AddComponent<Frosty::ECS::CMesh>(sword, Frosty::AssetManager::GetMesh("pCube1"));
		m_World->AddComponent<Frosty::ECS::CMaterial>(sword, Frosty::AssetManager::GetShader("FlatColor"));
		m_World->AddComponent<Frosty::ECS::CPhysics>(sword, Frosty::AssetManager::GetBoundingBox("pCube1"));

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

		glm::vec3 spawnScale = glm::vec3(10.0f, 6.0f, 10.0f);
		auto& sword = m_World->CreateEntity({ spawnPos.x, 3.0f, spawnPos.z }, { 0.f, 0.f, 0.f }, spawnScale);

		m_World->AddComponent<Frosty::ECS::CMesh>(sword, Frosty::AssetManager::GetMesh("pCube1"));
		m_World->AddComponent<Frosty::ECS::CMaterial>(sword, Frosty::AssetManager::GetShader("FlatColor"));
		m_World->AddComponent<Frosty::ECS::CPhysics>(sword, Frosty::AssetManager::GetBoundingBox("pCube1"));

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
		glm::vec3 spawnScale = glm::vec3(2.0f, 6.0f, 10.0f);
		auto& sword = m_World->CreateEntity({ spawnPos.x, 3.0f, spawnPos.z }, attackerTransform.Rotation, spawnScale);

		m_World->AddComponent<Frosty::ECS::CMesh>(sword, Frosty::AssetManager::GetMesh("pCube1"));
		m_World->AddComponent<Frosty::ECS::CMaterial>(sword, Frosty::AssetManager::GetShader("FlatColor"));
		m_World->AddComponent<Frosty::ECS::CPhysics>(sword, Frosty::AssetManager::GetBoundingBox("pCube1"));

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
		auto& projectile = m_World->CreateEntity({ spawnPos.x, 1.0f, spawnPos.z }, attackerTransform.Rotation, { 5.0f, 5.0f, 2.0f });
		m_World->AddComponent<Frosty::ECS::CMesh>(projectile, Frosty::AssetManager::GetMesh("player_arrow"));
		m_World->AddComponent<Frosty::ECS::CMaterial>(projectile, Frosty::AssetManager::GetShader("FlatColor"));
		auto& projectilePhysics = m_World->AddComponent<Frosty::ECS::CPhysics>(projectile, Frosty::AssetManager::GetBoundingBox("player_arrow"), weaponComp.ProjectileSpeed);
		projectilePhysics.Direction = direction;

		float criticalHit = 0;
		criticalHit = GenerateCriticalHit(weaponComp.CriticalHit, weaponComp.CriticalHitChance + weaponComp.FireCriticalHitChance);

		int totalDamage = int(glm::round(weaponComp.Damage + criticalHit + weaponComp.EarthDamage));

		m_World->AddComponent<Frosty::ECS::CAttack>(projectile, Frosty::ECS::CAttack::AttackType::Range, totalDamage, true, weaponComp.Lifetime);
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
			auto& projectile = m_World->CreateEntity({ spawnPos.x, 1.0f, spawnPos.z }, attackerTransform.Rotation, { 5.0f, 5.0f, 2.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(projectile, Frosty::AssetManager::GetMesh("player_arrow"));
			m_World->AddComponent<Frosty::ECS::CMaterial>(projectile, Frosty::AssetManager::GetShader("FlatColor"));
			auto& projectilePhysics = m_World->AddComponent<Frosty::ECS::CPhysics>(projectile, Frosty::AssetManager::GetBoundingBox("player_arrow"), weaponComp.ProjectileSpeed);
			projectilePhysics.Direction = direction;

			float criticalHit = 0;
			criticalHit = GenerateCriticalHit(weaponComp.CriticalHit, weaponComp.CriticalHitChance + weaponComp.FireCriticalHitChance);

			int totalDamage = int(glm::round(weaponComp.Damage + criticalHit + weaponComp.EarthDamage));

			m_World->AddComponent<Frosty::ECS::CAttack>(projectile, Frosty::ECS::CAttack::AttackType::Range, totalDamage, true, weaponComp.Lifetime);
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
		auto& projectile = m_World->CreateEntity({ spawnPos.x, 1.0f, spawnPos.z }, attackerTransform.Rotation, { 5.0f, 5.0f, 2.0f });
		m_World->AddComponent<Frosty::ECS::CMesh>(projectile, Frosty::AssetManager::GetMesh("player_arrow"));
		m_World->AddComponent<Frosty::ECS::CMaterial>(projectile, Frosty::AssetManager::GetShader("FlatColor"));
		auto& projectilePhysics = m_World->AddComponent<Frosty::ECS::CPhysics>(projectile, Frosty::AssetManager::GetBoundingBox("player_arrow"), weaponComp.ProjectileSpeed);
		projectilePhysics.Direction = direction;

		float criticalHit = 0;
		criticalHit = GenerateCriticalHit(weaponComp.CriticalHit, weaponComp.CriticalHitChance + weaponComp.FireCriticalHitChance);

		int totalDamage = int(glm::round(weaponComp.Damage + criticalHit + weaponComp.EarthDamage));

		m_World->AddComponent<Frosty::ECS::CAttack>(projectile, Frosty::ECS::CAttack::AttackType::Range, totalDamage, true, weaponComp.Lifetime, false);
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

				// CheckWitchCircle
				if (m_Inventory[index]->CurrentWolfsbane > 0)
				{
					// Send event to start the hexCircle timer (send the one requesting the enchantment)
					Frosty::EventBus::GetEventBus()->Publish<Frosty::ActivateWitchCircleEvent>(Frosty::ActivateWitchCircleEvent(m_Player[index]->EntityPtr));
				}
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
			}
		}
#pragma endregion Bait
	}

	void PlayerControllerSystem::OnPickUpEvent(Frosty::PickUpEvent& e)
	{
		auto world = Frosty::Application::Get().GetWorld().get();
		for (size_t i = 1; i < p_Total; i++)
		{
			auto& type = world->GetComponent<Frosty::ECS::CLootable>(e.GetEntity()).Type;
			auto& HUD = m_World->GetComponent<Frosty::ECS::CGUI>(m_Transform[i]->EntityPtr);

			if (type == Frosty::ECS::CLootable::LootType::HealingPotion)
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
			else if (type == Frosty::ECS::CLootable::LootType::IncHealthPotion)
			{
				if (m_Inventory[i]->CurrentIncreaseHPPotions < m_Inventory[i]->MaxIncreaseHPPotions && m_Health[i]->MaxHealth < m_Health[i]->MaxPossibleHealth)
				{
					m_Health[i]->MaxHealth += m_Inventory[i]->IncreaseHP;
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
			else if (type == Frosty::ECS::CLootable::LootType::SpeedPotion)
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
			else if (type == Frosty::ECS::CLootable::LootType::SpeedBoot)
			{
				if (m_Inventory[i]->CurrentSpeedBoots < m_Inventory[i]->MaxSpeedBoots)
				{
					if (m_Inventory[i]->IncreaseSpeed <= (m_Physics[i]->MaxSpeed - m_Physics[i]->Speed))
					{
						m_Inventory[i]->CurrentSpeedBoots++;
						m_Physics[i]->Speed += m_Inventory[i]->IncreaseSpeed;
						SetPickUpText(i, "Speed Increased");
					}
					FY_INFO("SpeedBoots Activated");
					FY_INFO("{0} / {1}", m_Inventory[i]->CurrentSpeedBoots, m_Inventory[i]->MaxSpeedBoots);
					if (!world->HasComponent<Frosty::ECS::CDestroy>(e.GetEntity()))
					{
						world->AddComponent<Frosty::ECS::CDestroy>(e.GetEntity());
					}
				}
				else
				{
					m_Physics[i]->Speed += m_Physics[i]->MaxSpeed;
				}
				FY_INFO("SpeedBoots Activated");
				FY_INFO("{0} / {1}", m_Inventory[i]->CurrentSpeedBoots, m_Inventory[i]->MaxSpeedBoots);
				if (!world->HasComponent<Frosty::ECS::CDestroy>(e.GetEntity()))
				{
					SetPickUpText(i, "Can't Pick Up SpeedBoots");
				}
			}
			else if (type == Frosty::ECS::CLootable::LootType::Sword1)
			{
				FY_INFO("Sword1");
				SetPickUpText(i, "Picked Up Sword Level 1");
				SwapWeapon(m_Player[i]->Weapon->EntityPtr, e.GetEntity());

				ResetAllHUDWeaponInfo(i);
				HUD.Layout.sprites.at(1).SetImage("attackMelee");
				HUD.Layout.sprites.at(2).SetImage("attackMelee1");
				HUD.Layout.sprites.at(3).SetImage("attackMelee2");
				HUD.Layout.sprites.at(4).SetImage("attackMelee3");

			}
			else if (type == Frosty::ECS::CLootable::LootType::Sword2)
			{
				FY_INFO("Sword2");
				SetPickUpText(i, "Picked Up Sword Level 2");
				SwapWeapon(m_Player[i]->Weapon->EntityPtr, e.GetEntity());

				ResetAllHUDWeaponInfo(i);
				HUD.Layout.sprites.at(1).SetImage("attackMelee");
				HUD.Layout.sprites.at(2).SetImage("attackMelee1");
				HUD.Layout.sprites.at(3).SetImage("attackMelee2");
				HUD.Layout.sprites.at(4).SetImage("attackMelee3");
			}
			else if (type == Frosty::ECS::CLootable::LootType::Sword3)
			{
				FY_INFO("Sword3");
				SetPickUpText(i, "Picked Up Sword Level 3");
				SwapWeapon(m_Player[i]->Weapon->EntityPtr, e.GetEntity());

				ResetAllHUDWeaponInfo(i);
				HUD.Layout.sprites.at(1).SetImage("attackMelee");
				HUD.Layout.sprites.at(2).SetImage("attackMelee1");
				HUD.Layout.sprites.at(3).SetImage("attackMelee2");
				HUD.Layout.sprites.at(4).SetImage("attackMelee3");
			}
			else if (type == Frosty::ECS::CLootable::LootType::Bow1)
			{
				FY_INFO("Arrow1");
				SetPickUpText(i, "Picked Up Bow Level 1");
				SwapWeapon(m_Player[i]->Weapon->EntityPtr, e.GetEntity());

				ResetAllHUDWeaponInfo(i);

				HUD.Layout.sprites.at(1).SetImage("attackRanged");
				HUD.Layout.sprites.at(2).SetImage("attackRanged1");
				HUD.Layout.sprites.at(3).SetImage("attackRanged2");
				HUD.Layout.sprites.at(4).SetImage("attackRanged3");
			}
			else if (type == Frosty::ECS::CLootable::LootType::Bow2)
			{
				FY_INFO("Arrow2");
				SetPickUpText(i, "Picked Up Bow Level 2");
				SwapWeapon(m_Player[i]->Weapon->EntityPtr, e.GetEntity());

				ResetAllHUDWeaponInfo(i);
				HUD.Layout.sprites.at(1).SetImage("attackRanged");
				HUD.Layout.sprites.at(2).SetImage("attackRanged1");
				HUD.Layout.sprites.at(3).SetImage("attackRanged2");
				HUD.Layout.sprites.at(4).SetImage("attackRanged3");
			}
			else if (type == Frosty::ECS::CLootable::LootType::Bow3)
			{
				FY_INFO("Arrow3");
				SetPickUpText(i, "Picked Up Bow Level 3");
				SwapWeapon(m_Player[i]->Weapon->EntityPtr, e.GetEntity());

				ResetAllHUDWeaponInfo(i);
				HUD.Layout.sprites.at(1).SetImage("attackRanged");
				HUD.Layout.sprites.at(2).SetImage("attackRanged1");
				HUD.Layout.sprites.at(3).SetImage("attackRanged2");
				HUD.Layout.sprites.at(4).SetImage("attackRanged3");
			}
		}
	}

	void PlayerControllerSystem::OnUpgradeWeaponEvent()
	{
		auto& weaponComp = m_World->GetComponent<Frosty::ECS::CWeapon>(m_Player[p_Total - 1]->Weapon->EntityPtr);

		// If player weapon is NOT fully upgraded --> proceed
		if (!weaponComp.IsFullyUpgraded)
		{
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
				if (weaponComp.FireCriticalHitChance == 0.f)	weaponComp.FireCriticalHitChance += 0.1f;
				break;
			case 1:
				if (weaponComp.EarthDamage == 0.f)	weaponComp.EarthDamage += 1.f;
				break;
			case 2:
				if (weaponComp.WindSpeed == 0.f)	weaponComp.WindSpeed += 0.4f;
				break;
			case 3:
				if (weaponComp.WaterHealing == 0)	weaponComp.WaterHealing += 1;
				break;
			default:
				break;
			}

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
						m_Health[i]->CurrentHealth += m_Inventory[i]->Heal;
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

	void PlayerControllerSystem::SwapWeapon(const std::shared_ptr<Frosty::ECS::Entity>& playerWeapon, const std::shared_ptr<Frosty::ECS::Entity>& lootWeapon)
	{
		// Swap CWeapon
		if ((m_World->HasComponent<Frosty::ECS::CWeapon>(playerWeapon)) && (m_World->HasComponent<Frosty::ECS::CWeapon>(lootWeapon)))
		{
			// Swap loot type in lootWeapon depending on playerWeapon
			SwapLootType(playerWeapon, lootWeapon);

			Frosty::ECS::CWeapon tempWeapon;
			tempWeapon = m_World->GetComponent<Frosty::ECS::CWeapon>(playerWeapon);
			m_World->GetComponent<Frosty::ECS::CWeapon>(playerWeapon) = m_World->GetComponent<Frosty::ECS::CWeapon>(lootWeapon);
			m_World->GetComponent<Frosty::ECS::CWeapon>(lootWeapon) = tempWeapon;

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
				Frosty::ECS::CMesh tempMesh;
				tempMesh = m_World->GetComponent<Frosty::ECS::CMesh>(playerWeapon);
				m_World->GetComponent<Frosty::ECS::CMesh>(playerWeapon) = m_World->GetComponent<Frosty::ECS::CMesh>(lootWeapon);
				m_World->GetComponent<Frosty::ECS::CMesh>(lootWeapon) = tempMesh;

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
				Frosty::ECS::CMaterial tempMaterial;
				tempMaterial = m_World->GetComponent<Frosty::ECS::CMaterial>(playerWeapon);
				m_World->GetComponent<Frosty::ECS::CMaterial>(playerWeapon) = m_World->GetComponent<Frosty::ECS::CMaterial>(lootWeapon);
				m_World->GetComponent<Frosty::ECS::CMaterial>(lootWeapon) = tempMaterial;
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
				lootComp.Type = Frosty::ECS::CLootable::LootType::Sword1;
			else if (level == 2)
				lootComp.Type = Frosty::ECS::CLootable::LootType::Sword2;
			else if (level == 3)
				lootComp.Type = Frosty::ECS::CLootable::LootType::Sword3;
		}
		else if (playerWeaponComp.Type == Frosty::ECS::CWeapon::WeaponType::Bow)
		{
			if (level == 1)
				lootComp.Type = Frosty::ECS::CLootable::LootType::Bow1;
			else if (level == 2)
				lootComp.Type = Frosty::ECS::CLootable::LootType::Bow2;
			else if (level == 3)
				lootComp.Type = Frosty::ECS::CLootable::LootType::Bow3;
		}
	}

	void PlayerControllerSystem::UpdateHUD(size_t index)
	{
		if (m_World->HasComponent<Frosty::ECS::CGUI>(m_Transform[index]->EntityPtr))
		{
			auto& HUD = m_World->GetComponent<Frosty::ECS::CGUI>(m_Transform[index]->EntityPtr);
			//Items
			HUD.Layout.texts.at(0).SetText(std::string(std::to_string(m_Inventory[index]->CurrentHealingPotions) + "/" + std::string(std::to_string(m_Inventory[index]->MaxHealingPotions))));
			HUD.Layout.texts.at(1).SetText(std::string(std::to_string(m_Inventory[index]->CurrentSpeedPotions) + "/" + std::string(std::to_string(m_Inventory[index]->MaxSpeedBoots))));
			HUD.Layout.texts.at(2).SetText(std::string(std::to_string(m_Inventory[index]->CurrentBaitAmount) + "/" + std::string(std::to_string(m_Inventory[index]->MaxBaitAmount))));
			HUD.Layout.texts.at(3).SetText(std::string(std::to_string(m_Inventory[index]->CurrentWolfsbane)));

			if (m_Inventory[index]->CurrentHealingPotions == 0)
			{
				HUD.Layout.sprites.at(9).SetColorSprite(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
				HUD.Layout.texts.at(18).SetText(std::string(""));
				HUD.Layout.texts.at(0).SetColor(glm::vec3(0.75f, 0.75f, 0.50f));
			}
			else
			{
				HUD.Layout.sprites.at(9).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				HUD.Layout.texts.at(18).SetText(std::string("[1]"));
				HUD.Layout.texts.at(0).SetColor(glm::vec3(1.0f, 1.0f, 0.75f));

			}

			if (m_Inventory[index]->CurrentSpeedPotions == 0)
			{
				HUD.Layout.sprites.at(10).SetColorSprite(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
				HUD.Layout.texts.at(19).SetText(std::string(""));
				HUD.Layout.texts.at(1).SetColor(glm::vec3(0.75f, 0.75f, 0.50f));

			}
			else
			{
				HUD.Layout.sprites.at(10).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				HUD.Layout.texts.at(19).SetText(std::string("[2]"));
				HUD.Layout.texts.at(1).SetColor(glm::vec3(1.0f, 1.0f, 0.75f));

			}

			if (m_Inventory[index]->CurrentBaitAmount == 0)
			{
				HUD.Layout.sprites.at(11).SetColorSprite(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
				HUD.Layout.texts.at(20).SetText(std::string(""));
				HUD.Layout.texts.at(2).SetColor(glm::vec3(0.5f, 0.5f, 0.25f));

			}
			else
			{
				HUD.Layout.sprites.at(11).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				HUD.Layout.texts.at(20).SetText(std::string("[Q]"));
				HUD.Layout.texts.at(2).SetColor(glm::vec3(1.0f, 1.0f, 0.75f));

			}

			//Points
			HUD.Layout.texts.at(4).SetText(std::string("Points: " + std::to_string(m_Player[index]->Score)));

			//Temp Health
			//HUD.Layout.texts.at(5).SetText(std::string(std::to_string((int)m_Health[index]->CurrentHealth) + "/" + std::to_string((int)m_Health[index]->MaxHealth)));

			int nrOfFilledHearts = m_Health[index]->CurrentHealth / 4;
			int nrOfHeartQuadrants = m_Health[index]->CurrentHealth % 4;

			int healthSpriteID = 19;
			for (int i = 0; i < nrOfFilledHearts && nrOfFilledHearts <= m_Health[index]->MaxHealth; i++)
			{
				if (healthSpriteID < 29)
				{
					HUD.Layout.sprites.at(healthSpriteID).SetImage("Heart_4");
					HUD.Layout.sprites.at(healthSpriteID).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					healthSpriteID++;
				}
			}

			if (nrOfHeartQuadrants == 1)
			{
				if (healthSpriteID < 29)
				{
					HUD.Layout.sprites.at(healthSpriteID).SetImage("Heart_1");
					HUD.Layout.sprites.at(healthSpriteID).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}
			else if (nrOfHeartQuadrants == 2)
			{
				if (healthSpriteID < 29)
				{
					HUD.Layout.sprites.at(healthSpriteID).SetImage("Heart_2");
					HUD.Layout.sprites.at(healthSpriteID).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}
			else if (nrOfHeartQuadrants == 3)
			{
				if (healthSpriteID < 29)
				{
					HUD.Layout.sprites.at(healthSpriteID).SetImage("Heart_3");
					HUD.Layout.sprites.at(healthSpriteID).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}
			else if (m_Health[index]->CurrentHealth < m_Health[index]->MaxHealth)
			{
				if (healthSpriteID < 29)
				{
					HUD.Layout.sprites.at(healthSpriteID).SetImage("Heart_0");
					HUD.Layout.sprites.at(healthSpriteID).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}

			int nrOfEmptyHearts = (m_Health[index]->MaxHealth - m_Health[index]->CurrentHealth) / 4;
			healthSpriteID = 19 + m_Health[index]->CurrentHealth / 4;

			if (nrOfHeartQuadrants > 0)
			{
				healthSpriteID++;
			}

			for (int i = 0; i < nrOfEmptyHearts; i++)
			{
				if (healthSpriteID < 29)
				{
					HUD.Layout.sprites.at(healthSpriteID).SetImage("Heart_0");
					HUD.Layout.sprites.at(healthSpriteID).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					healthSpriteID++;
				}

			}

				//Pickup Text
			if (Frosty::Time::CurrentTime() - m_Player[index]->PickUpTextTimer >= m_Player[index]->PickUpTextTime)
			{
				HUD.Layout.texts.at(6).SetText("");
			}

			//Attack cooldown
			auto& weapon = m_Player[index]->Weapon;
			if (weapon->Level == 3)
			{
				HUD.Layout.texts.at(14).SetText(std::string("[SPACE]"));

				float timer = weapon->LVL3AttackCooldownTimer;
				float time = Frosty::Time::CurrentTime();
				float dif = (time - timer);
				float cooldown = weapon->LVL3AttackCooldown - dif;
				if (cooldown > 0)
				{
					int cooldown1 = (int)cooldown;
					int cooldown2 = (int)((cooldown - cooldown1) * 10);
					HUD.Layout.texts.at(7).SetText(std::string(std::to_string(cooldown1) + "." + std::to_string(cooldown2)));
					HUD.Layout.sprites.at(4).SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.90f));
				}
				else
				{
					HUD.Layout.texts.at(7).SetText(std::string(""));
					HUD.Layout.sprites.at(4).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

				}
			}
			else
			{
				HUD.Layout.texts.at(14).SetText(std::string(""));
				HUD.Layout.sprites.at(4).SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));

			}

			if (weapon->Level >= 2)
			{
				HUD.Layout.texts.at(15).SetText(std::string("[R-MOUSE]"));

				float timer = weapon->LVL2AttackCooldownTimer;
				float time = Frosty::Time::CurrentTime();
				float dif = (time - timer);
				float cooldown = weapon->LVL2AttackCooldown - dif;
				if (cooldown > 0)
				{
					int cooldown1 = (int)cooldown;
					int cooldown2 = (int)((cooldown - cooldown1) * 10);
					HUD.Layout.texts.at(8).SetText(std::string(std::to_string(cooldown1) + "." + std::to_string(cooldown2)));
					HUD.Layout.sprites.at(3).SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.90f));


				}
				else
				{
					HUD.Layout.texts.at(8).SetText(std::string(""));
					HUD.Layout.sprites.at(3).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

				}
			}
			else
			{
				HUD.Layout.texts.at(15).SetText(std::string(""));
				HUD.Layout.sprites.at(3).SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));

			}

			if (weapon->Level >= 1)
			{
				HUD.Layout.texts.at(16).SetText(std::string("[L-MOUSE]"));

				float timer = weapon->LVL1AttackCooldownTimer;
				float time = Frosty::Time::CurrentTime();
				float dif = (time - timer);
				float cooldown = weapon->LVL1AttackCooldown - dif;
				if (cooldown > 0)
				{
					int cooldown1 = (int)cooldown;
					int cooldown2 = (int)((cooldown - cooldown1) * 10);
					HUD.Layout.texts.at(9).SetText(std::string(std::to_string(cooldown1) + "." + std::to_string(cooldown2)));
					HUD.Layout.sprites.at(2).SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.90f));


				}
				else
				{
					HUD.Layout.texts.at(9).SetText(std::string(""));
					HUD.Layout.sprites.at(2).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

				}

			}
			else
			{
				HUD.Layout.texts.at(16).SetText(std::string(""));
				HUD.Layout.sprites.at(2).SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));

			}


			//Dash cooldown
			if (m_Dash[index]->CurrentCooldown > 0)
			{
				int cooldown1 = (int)(m_Dash[index]->CurrentCooldown);
				int cooldown2 = (int)(((m_Dash[index]->CurrentCooldown - cooldown1) * 10));

				HUD.Layout.texts.at(10).SetText(std::string(std::to_string(cooldown1) + "." + std::to_string(cooldown2)));
				HUD.Layout.sprites.at(13).SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.90f));

			}
			else
			{
				HUD.Layout.texts.at(10).SetText(std::string(""));
				HUD.Layout.sprites.at(13).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

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
				HUD.Layout.texts.at(11).SetText(std::string(std::to_string(cooldown1) + "." + std::to_string(cooldown2)));
				HUD.Layout.sprites.at(9).SetColorSprite(glm::vec4(0.2f, 0.2f, 0.2f, 0.75f));

			}
			else
			{
				HUD.Layout.texts.at(11).SetText(std::string(""));
				if (m_Inventory[index]->CurrentHealingPotions != 0)
				{
					HUD.Layout.sprites.at(9).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
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
				HUD.Layout.texts.at(12).SetText(std::string(std::to_string(cooldown1) + "." + std::to_string(cooldown2)));
				HUD.Layout.sprites.at(10).SetColorSprite(glm::vec4(0.2f, 0.2f, 0.2f, 0.75f));

			}
			else
			{
				HUD.Layout.texts.at(12).SetText(std::string(""));
				if (m_Inventory[index]->CurrentSpeedPotions != 0)
				{
					HUD.Layout.sprites.at(10).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
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
				HUD.Layout.texts.at(13).SetText(std::string(std::to_string(cooldown1) + "." + std::to_string(cooldown2)));
				HUD.Layout.sprites.at(11).SetColorSprite(glm::vec4(0.2f, 0.2f, 0.2f, 0.75f));

			}
			else
			{
				HUD.Layout.texts.at(13).SetText(std::string(""));
				if (m_Inventory[index]->CurrentBaitAmount != 0)
				{
					HUD.Layout.sprites.at(11).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				}

			}

			//Speed boots
			int bootSpriteID = 14;
			for (int i = 0; i < m_Inventory[index]->CurrentSpeedBoots && m_Inventory[index]->CurrentSpeedBoots <= 5; i++)
			{

				HUD.Layout.sprites.at(bootSpriteID).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				bootSpriteID++;
			}

			//Elemental

			//Earth
			if (m_Player[index]->Weapon->FireCriticalHitChance > 0.0f)
			{
				HUD.Layout.sprites.at(5).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			else
			{
				HUD.Layout.sprites.at(5).SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));
			}
			//Fire
			if (m_Player[index]->Weapon->FireCriticalHitChance > 0.0f)
			{
				HUD.Layout.sprites.at(6).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			else
			{
				HUD.Layout.sprites.at(6).SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));
			}
			//Water
			if (m_Player[index]->Weapon->FireCriticalHitChance > 0.0f)
			{
				HUD.Layout.sprites.at(7).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			else
			{
				HUD.Layout.sprites.at(7).SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));
			}
			//Wind
			if (m_Player[index]->Weapon->FireCriticalHitChance > 0.0f)
			{
				HUD.Layout.sprites.at(8).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			else
			{
				HUD.Layout.sprites.at(8).SetColorSprite(glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));
			}

		}
	}

	void PlayerControllerSystem::SetPickUpText(size_t index, std::string text)
	{
		if (m_World->HasComponent<Frosty::ECS::CGUI>(m_Transform[index]->EntityPtr))
		{
			auto& HUD = m_World->GetComponent<Frosty::ECS::CGUI>(m_Transform[index]->EntityPtr);

			HUD.Layout.texts.at(6).SetText(text);
			m_Player[index]->PickUpTextTimer = Frosty::Time::CurrentTime();

		}
	}

	void PlayerControllerSystem::ResetAllHUDWeaponInfo(size_t index)
	{
		auto& HUD = m_World->GetComponent<Frosty::ECS::CGUI>(m_Transform[index]->EntityPtr);
		HUD.Layout.texts.at(7).SetText(std::string(""));
		HUD.Layout.texts.at(8).SetText(std::string(""));
		HUD.Layout.texts.at(9).SetText(std::string(""));
	}
}