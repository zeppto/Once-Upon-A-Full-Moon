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

	void PlayerControllerSystem::OnEvent(Frosty::BaseEvent& e)
	{
		switch (e.GetEventType())
		{
		case Frosty::EventType::PickUp:
			OnPickUpEvent(static_cast<Frosty::PickUpEvent&>(e));
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
		// The entity pointing to a CWeapon component (player / enemy)
		auto& weaponCarrier = m_Player[index]->EntityPtr;

		// The weapon that the player is wielding
		auto& weaponComp = (m_World->GetComponent<Frosty::ECS::CWeapon>(m_Player[index]->Weapon->EntityPtr));

		if (Frosty::InputManager::IsMouseButtonPressed(m_Player[index]->LVL1Attack))
		{
			// LVL: 1, 2, 3
			if (weaponComp.ItemID == 1 || weaponComp.ItemID == 2 || weaponComp.ItemID == 3)
			{
				LVL1Attack(index);
			}
		}
		else if (Frosty::InputManager::IsMouseButtonPressed(m_Player[index]->LVL2Attack))
		{
			// LVL: 1, 2
			if (weaponComp.ItemID == 2 || weaponComp.ItemID == 3)
			{
				LVL2Attack(index);
			}
		}
		else if (Frosty::InputManager::IsKeyPressed(m_Player[index]->LVL3Attack))
		{
			// LVL: 1, 2, 3
			if (weaponComp.ItemID == 3)
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

		if (Frosty::Time::CurrentTime() - weaponComp.LVL1AttackCooldownTimer >= weaponComp.LVL1AttackCooldown)
		{
			switch (weaponComp.Type)
			{
			case Frosty::ECS::CWeapon::WeaponType::Sword:
				CreateLVL1BoundingBox(weaponCarrier, weaponComp.EntityPtr);
				break;
			case Frosty::ECS::CWeapon::WeaponType::Arrow:
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

		if (Frosty::Time::CurrentTime() - weaponComp.LVL2AttackCooldownTimer >= weaponComp.LVL2AttackCooldown)
		{
			switch (weaponComp.Type)
			{
			case Frosty::ECS::CWeapon::WeaponType::Sword:
				CreateLVL2BoundingBox(weaponCarrier, weaponComp.EntityPtr);
				break;
			case Frosty::ECS::CWeapon::WeaponType::Arrow:
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

		if (Frosty::Time::CurrentTime() - weaponComp.LVL3AttackCooldownTimer >= weaponComp.LVL3AttackCooldown)
		{
			switch (weaponComp.Type)
			{
			case Frosty::ECS::CWeapon::WeaponType::Sword:
				CreateLVL3BoundingBox(weaponCarrier, weaponComp.EntityPtr);
				break;
			case Frosty::ECS::CWeapon::WeaponType::Arrow:
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
		criticalHit = GenerateCriticalHit(weaponComp.Damage, weaponComp.CriticalHit);

		m_World->AddComponent<Frosty::ECS::CAttack>(sword, Frosty::ECS::CAttack::AttackType::Melee, weaponComp.Damage + criticalHit, true); // <-- true in the end because it's a friendly attack
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
		criticalHit = GenerateCriticalHit(weaponComp.Damage, weaponComp.CriticalHit);

		m_World->AddComponent<Frosty::ECS::CAttack>(sword, Frosty::ECS::CAttack::AttackType::Melee, weaponComp.Damage + criticalHit, true); // <-- true in the end because it's a friendly attack
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
		criticalHit = GenerateCriticalHit(weaponComp.Damage, weaponComp.CriticalHit);

		m_World->AddComponent<Frosty::ECS::CAttack>(sword, Frosty::ECS::CAttack::AttackType::Melee, weaponComp.Damage + criticalHit, true); // <-- true in the end because it's a friendly attack
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
		auto& projectile = m_World->CreateEntity({ spawnPos.x, 1.0f, spawnPos.z }, attackerTransform.Rotation, { 0.3f, 0.3f, 0.3f });
		m_World->AddComponent<Frosty::ECS::CMesh>(projectile, Frosty::AssetManager::GetMesh("pSphere1"));
		m_World->AddComponent<Frosty::ECS::CMaterial>(projectile, Frosty::AssetManager::GetShader("FlatColor"));
		auto& projectilePhysics = m_World->AddComponent<Frosty::ECS::CPhysics>(projectile, Frosty::AssetManager::GetBoundingBox("pSphere1"), 20.0f);
		projectilePhysics.Velocity = direction * projectilePhysics.Speed;

		float criticalHit = 0;
		criticalHit = GenerateCriticalHit(weaponComp.Damage, weaponComp.CriticalHit);
		m_World->AddComponent<Frosty::ECS::CAttack>(projectile, Frosty::ECS::CAttack::AttackType::Range, weaponComp.Damage + criticalHit, true, weaponComp.Lifetime);
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
		float criticalHit = 0;
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
			auto& projectile = m_World->CreateEntity({ spawnPos.x, 1.0f, spawnPos.z }, attackerTransform.Rotation, { 0.3f, 0.3f, 0.3f });
			m_World->AddComponent<Frosty::ECS::CMesh>(projectile, Frosty::AssetManager::GetMesh("pSphere1"));
			m_World->AddComponent<Frosty::ECS::CMaterial>(projectile, Frosty::AssetManager::GetShader("FlatColor"));
			auto& projectilePhysics = m_World->AddComponent<Frosty::ECS::CPhysics>(projectile, Frosty::AssetManager::GetBoundingBox("pSphere1"), 20.0f);
			projectilePhysics.Velocity = direction * projectilePhysics.Speed;

			criticalHit = GenerateCriticalHit(weaponComp.Damage, weaponComp.CriticalHit);
			m_World->AddComponent<Frosty::ECS::CAttack>(projectile, Frosty::ECS::CAttack::AttackType::Range, weaponComp.Damage + criticalHit, true, weaponComp.Lifetime);
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
		auto& projectile = m_World->CreateEntity({ spawnPos.x, 1.0f, spawnPos.z }, attackerTransform.Rotation, { 0.3f, 0.3f, 0.3f });
		m_World->AddComponent<Frosty::ECS::CMesh>(projectile, Frosty::AssetManager::GetMesh("pSphere1"));
		m_World->AddComponent<Frosty::ECS::CMaterial>(projectile, Frosty::AssetManager::GetShader("FlatColor"));
		auto& projectilePhysics = m_World->AddComponent<Frosty::ECS::CPhysics>(projectile, Frosty::AssetManager::GetBoundingBox("pSphere1"), 20.0f);
		projectilePhysics.Velocity = direction * projectilePhysics.Speed;

		float criticalHit = 0;
		criticalHit = GenerateCriticalHit(weaponComp.Damage, weaponComp.CriticalHit);
		m_World->AddComponent<Frosty::ECS::CAttack>(projectile, Frosty::ECS::CAttack::AttackType::Range, weaponComp.Damage + criticalHit, true, weaponComp.Lifetime, false);
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
		if (Frosty::InputManager::IsKeyPressed(FY_KEY_E))
		{
			if (!m_keyPressed)
			{
				m_keyPressed = true;
				Frosty::EventBus::GetEventBus()->Publish<Frosty::PickUpAttemptEvent>(Frosty::PickUpAttemptEvent(m_Player[index]->EntityPtr));
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
				world->AddComponent<Frosty::ECS::CMesh>(bait, Frosty::AssetManager::GetMesh("scarlet"));
				auto& material = world->AddComponent<Frosty::ECS::CMaterial>(bait, Frosty::AssetManager::GetShader("Texture2D"));
				material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
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

			if (type == Frosty::ECS::CLootable::LootType::HealingPotion && m_Inventory[i]->CurrentHealingPotions < m_Inventory[i]->MaxHealingPotions)
			{
				m_Inventory[i]->CurrentHealingPotions++;
				FY_INFO("HealingPotion in Inventory");
				FY_INFO("{0} / {1}", m_Inventory[i]->CurrentHealingPotions, m_Inventory[i]->MaxHealingPotions);
				if (!world->HasComponent<Frosty::ECS::CDestroy>(e.GetEntity()))
				{
					world->AddComponent<Frosty::ECS::CDestroy>(e.GetEntity());
				}
			}
			else if (type == Frosty::ECS::CLootable::LootType::IncHealthPotion && m_Inventory[i]->CurrentIncreaseHPPotions < m_Inventory[i]->MaxIncreaseHPPotions)
			{
				m_Health[i]->MaxHealth += 3;
				FY_INFO("Health Increased");
				//FY_INFO("{0} / {1}", m_Inventory[i]->CurrentIncreaseHPPotions, m_Inventory[i]->MaxIncreaseHPPotions);
				if (!world->HasComponent<Frosty::ECS::CDestroy>(e.GetEntity()))
				{
					world->AddComponent<Frosty::ECS::CDestroy>(e.GetEntity());
				}
			}
			else if (type == Frosty::ECS::CLootable::LootType::SpeedPotion && m_Inventory[i]->CurrentSpeedPotions < m_Inventory[i]->MaxSpeedPotions)
			{
				m_Inventory[i]->CurrentSpeedPotions++;
				FY_INFO("SpeedPotion in Inventory");
				FY_INFO("{0} / {1}", m_Inventory[i]->CurrentSpeedPotions, m_Inventory[i]->MaxSpeedPotions);
				if (!world->HasComponent<Frosty::ECS::CDestroy>(e.GetEntity()))
				{
					world->AddComponent<Frosty::ECS::CDestroy>(e.GetEntity());
				}
			}
			else if (type == Frosty::ECS::CLootable::LootType::SpeedBoot && m_Inventory[i]->CurrentSpeedBoots < m_Inventory[i]->MaxSpeedBoots)
			{
				if (m_Inventory[i]->IncreaseSpeed <= (m_Physics[i]->MaxSpeed - m_Physics[i]->Speed))
				{
					m_Physics[i]->Speed += m_Inventory[i]->IncreaseSpeed;
				}
				FY_INFO("SpeedBoots Activated");
				FY_INFO("{0} / {1}", m_Inventory[i]->CurrentSpeedBoots, m_Inventory[i]->MaxSpeedBoots);
				if (!world->HasComponent<Frosty::ECS::CDestroy>(e.GetEntity()))
				{
					world->AddComponent<Frosty::ECS::CDestroy>(e.GetEntity());
				}
			}
			else if (type == Frosty::ECS::CLootable::LootType::Sword1)
			{
				FY_INFO("Sword1");
			}
			else if (type == Frosty::ECS::CLootable::LootType::Arrow1)
			{
				FY_INFO("Arrow1");
			}
		}
	}
	
	void PlayerControllerSystem::SwapWeapon(const std::shared_ptr<Frosty::ECS::Entity>& playerWeapon, const std::shared_ptr<Frosty::ECS::Entity>& lootWeapon)
	{
		// Swap CWeapon
		if ((m_World->HasComponent<Frosty::ECS::CWeapon>(playerWeapon)) && (m_World->HasComponent<Frosty::ECS::CWeapon>(lootWeapon)))
		{
			if ((m_World->GetComponent<Frosty::ECS::CWeapon>(playerWeapon) != m_World->GetComponent<Frosty::ECS::CWeapon>(lootWeapon)))
			{
				Frosty::ECS::CWeapon tempWeapon;
				tempWeapon = m_World->GetComponent<Frosty::ECS::CWeapon>(playerWeapon);
				m_World->GetComponent<Frosty::ECS::CWeapon>(playerWeapon) = m_World->GetComponent<Frosty::ECS::CWeapon>(lootWeapon);
				m_World->GetComponent<Frosty::ECS::CWeapon>(lootWeapon) = tempWeapon;

				// Only switch CMesh and CMaterial when weapon stats have been swapped
				SwapMesh(playerWeapon, lootWeapon);
				SwapMaterial(playerWeapon, lootWeapon);
			}
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
}