#include <mcspch.hpp>
#include "AISystem.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"

namespace MCS
{
	const std::string AISystem::NAME = "AISystem";

	void AISystem::Init()
	{
		m_World = Frosty::Application::Get().GetWorld().get();

		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CEnemy>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
	}

	void AISystem::OnUpdate()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Enemy[i]->Target == nullptr) continue;

			CheckState(i);

			HandleAttack(i);

			//if (Frosty::Time::GetFrameCount() % 20 == 0)
			//{
			//	if (m_World->HasComponent<Frosty::ECS::CBoss>(m_Enemy[i]->EntityPtr));
			//	{
			//		auto& bossComp = m_World->GetComponent<Frosty::ECS::CBoss>(m_Enemy[i]->EntityPtr);
			//		if (bossComp.ActiveAbility == Frosty::ECS::CBoss::AbilityState::None) FY_INFO("AbilityState::None");
			//		else if (bossComp.ActiveAbility == Frosty::ECS::CBoss::AbilityState::Leap) FY_INFO("AbilityState::Leap");
			//		else if (bossComp.ActiveAbility == Frosty::ECS::CBoss::AbilityState::Charge) FY_INFO("AbilityState::Charge");
			//	}
			//}
		}
	}

	void AISystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Enemy[p_Total] = &world->GetComponent<Frosty::ECS::CEnemy>(entity);
			m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);

			p_Total++;
		}
	}

	void AISystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_Enemy[p_Total] = nullptr;
			m_Health[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void AISystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CEnemy* enemyPtr = world->GetComponentAddress<Frosty::ECS::CEnemy>(entity);
			Frosty::ECS::CHealth* healthPtr = world->GetComponentAddress<Frosty::ECS::CHealth>(entity);

			m_Transform[it->second] = transformPtr;
			m_Enemy[it->second] = enemyPtr;
			m_Health[it->second] = healthPtr;
		}
	}

	std::string AISystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_Enemy[i] << "\t" << m_Enemy[i]->EntityPtr->Id << "\t\t" << m_Enemy[i]->EntityPtr << "\t\t" << m_Enemy[i]->EntityPtr.use_count() << "\n";
			retInfo << "\t\t" << i << "\t" << m_Health[i] << "\t" << m_Health[i]->EntityPtr->Id << "\t\t" << m_Health[i]->EntityPtr << "\t\t" << m_Health[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}
	
	void AISystem::CheckState(size_t index)
	{
		// Reset (Should not happen if boss)
		if (glm::distance(m_Transform[index]->Position, m_Enemy[index]->SpawnPosition) >= (float)Frosty::ECS::CEnemy::RESET_DISTANCE &&
			!m_World->HasComponent<Frosty::ECS::CBoss>(m_Enemy[index]->EntityPtr))
		{
			m_Enemy[index]->CurrentState = Frosty::ECS::CEnemy::State::Reset;
		}
		if (m_Enemy[index]->CurrentState == Frosty::ECS::CEnemy::State::Reset)
		{
			if (glm::distance(m_Transform[index]->Position, m_Enemy[index]->SpawnPosition) <= 0.8f)
			{
				m_Enemy[index]->CurrentState = Frosty::ECS::CEnemy::State::Idle;
				m_Health[index]->CurrentHealth = m_Health[index]->MaxHealth;
				m_Transform[index]->Position = m_Enemy[index]->SpawnPosition;
			}
			else
			{
				//if (Frosty::Time::GetFrameCount() % 20 == 0) FY_INFO("Reset");
				return;
			}
		}

		// Escape
		if (m_Health[index]->CurrentHealth <= m_Health[index]->MaxHealth * m_Enemy[index]->RunOnHealth)
		{
			m_Enemy[index]->CurrentState = Frosty::ECS::CEnemy::State::Escape;
			//if (Frosty::Time::GetFrameCount() % 20 == 0) FY_INFO("Escape");
			return;
		}

		// Idle
		if (glm::distance(m_Transform[index]->Position, m_Enemy[index]->Target->Position) > m_Enemy[index]->SightRange)
		{
			m_Enemy[index]->CurrentState = Frosty::ECS::CEnemy::State::Idle;
			//if (Frosty::Time::GetFrameCount() % 20 == 0) FY_INFO("Idle");
			return;
		}

		// Attack
		if (glm::distance(m_Transform[index]->Position, m_Enemy[index]->Target->Position) <= m_Enemy[index]->Weapon->MaxAttackRange)
		{
			m_Enemy[index]->CurrentState = Frosty::ECS::CEnemy::State::Attack;
			//if (Frosty::Time::GetFrameCount() % 20 == 0) FY_INFO("Attack");

			//if (glm::distance(m_Transform[index]->Position, m_Enemy[index]->Target->Position) < m_Enemy[index]->Weapon->MinAttackRange)
			//{
			//	KeepDistanceFromTarget(index, m_Enemy[index]->Weapon->MinAttackRange);
			//}

			return;
		}

		// Chase
		if (glm::distance(m_Transform[index]->Position, m_Enemy[index]->Target->Position) <= m_Enemy[index]->SightRange)
		{
			bool stopChase = HandleBossAbilities(index);
			if (!stopChase)
			{
				m_Enemy[index]->CurrentState = Frosty::ECS::CEnemy::State::Chase;
				//if (Frosty::Time::GetFrameCount() % 20 == 0) FY_INFO("Chase");
			}
		}
	}

	void AISystem::HandleAttack(size_t index)
	{
		// Preconditions
		if (m_Enemy[index]->CurrentState != Frosty::ECS::CEnemy::State::Attack) return;
		if (m_Enemy[index]->Target == nullptr) return;
		if (m_Enemy[index]->Weapon == nullptr) return;
		if (glm::distance(m_Transform[index]->Position, m_Enemy[index]->Target->Position) > m_Enemy[index]->Weapon->MaxAttackRange) return;
		if (HandleBossAbilities(index)) return;

		// Rotate towards player
		LookAtPoint(m_Enemy[index]->Target->Position, index);

		if (Frosty::Time::CurrentTime() - m_Enemy[index]->Weapon->LVL1AttackCooldownTimer >= m_Enemy[index]->Weapon->LVL1AttackCooldown)
		{
			// Calculate direction vector
			glm::mat4 mat = glm::mat4(1.0f);
			mat = glm::rotate(mat, glm::radians(m_Transform[index]->Rotation.x), { 1.0f, 0.0f, 0.0f });
			mat = glm::rotate(mat, glm::radians(m_Transform[index]->Rotation.y), { 0.0f, 1.0f, 0.0f });
			mat = glm::rotate(mat, glm::radians(m_Transform[index]->Rotation.z), { 0.0f, 0.0f, 1.0f });
			glm::vec4 direction = mat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0);

			// Calculate attack position and AoE
			auto& physComp = m_World->GetComponent<Frosty::ECS::CPhysics>(m_Transform[index]->EntityPtr);
			glm::vec3 modelSize = glm::vec3(physComp.BoundingBox->halfSize[0], physComp.BoundingBox->halfSize[1], physComp.BoundingBox->halfSize[2]) * m_Transform[index]->Scale;
			glm::vec3 spawnPos = m_Transform[index]->Position + (glm::vec3(direction) * (modelSize.z + m_Enemy[index]->Weapon->AttackHitboxScale.z * 0.5f));

			// Create Attack (BB)
			auto& attack = m_World->CreateEntity({ spawnPos.x, 3.0f, spawnPos.z }, m_Transform[index]->Rotation, m_Enemy[index]->Weapon->AttackHitboxScale);
			if (m_Enemy[index]->Weapon->Type == Frosty::ECS::CWeapon::WeaponType::Bow)
			{
				m_World->AddComponent<Frosty::ECS::CMesh>(attack, Frosty::AssetManager::GetMesh("pSphere1"));
				m_World->AddComponent<Frosty::ECS::CMaterial>(attack, Frosty::AssetManager::GetShader("FlatColor"));
				auto& physComp = m_World->AddComponent<Frosty::ECS::CPhysics>(attack, Frosty::AssetManager::GetBoundingBox("pSphere1"), 20.0f);
				m_World->AddComponent<Frosty::ECS::CAttack>(attack, Frosty::ECS::CAttack::AttackType::Range, (int)m_Enemy[index]->Weapon->Damage, false, m_Enemy[index]->Weapon->Lifetime);
				physComp.Direction = direction;
			}
			else
			{
				m_World->AddComponent<Frosty::ECS::CMesh>(attack, Frosty::AssetManager::GetMesh("pCube1"));						// Remove later
				m_World->AddComponent<Frosty::ECS::CMaterial>(attack, Frosty::AssetManager::GetShader("FlatColor"));			// Remove later
				m_World->AddComponent<Frosty::ECS::CPhysics>(attack, Frosty::AssetManager::GetBoundingBox("pCube1"), 0.0f);
				m_World->AddComponent<Frosty::ECS::CAttack>(attack, Frosty::ECS::CAttack::AttackType::Melee, (int)m_Enemy[index]->Weapon->Damage, false, m_Enemy[index]->Weapon->Lifetime);
			}

			// Set cool down
			m_Enemy[index]->Weapon->LVL1AttackCooldownTimer = Frosty::Time::CurrentTime();
		}
	}

	void AISystem::LookAtPoint(const glm::vec3& point, size_t index)
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

	bool AISystem::HandleBossAbilities(size_t index)
	{
		if (!m_World->HasComponent<Frosty::ECS::CBoss>(m_Enemy[index]->EntityPtr)) return false;
		auto& bossComp = m_World->GetComponent<Frosty::ECS::CBoss>(m_Enemy[index]->EntityPtr);
		auto& physComp = m_World->GetComponent<Frosty::ECS::CPhysics>(bossComp.EntityPtr);

		if (bossComp.ActiveAbility == Frosty::ECS::CBoss::AbilityState::None)
		{
			//int randomNr = rand() % 2;
			int randomNr = 1;
			if (randomNr == 0 && (Frosty::Time::CurrentTime() - bossComp.LeapCooldownTime >= bossComp.LeapInterval) &&
				glm::distance(m_Transform[index]->Position, m_Enemy[index]->Target->Position) <= bossComp.ChargeDistance)
			{
				// Leap
				bossComp.ActiveAbility = Frosty::ECS::CBoss::AbilityState::Leap;
			}
			else if (randomNr == 1 && (Frosty::Time::CurrentTime() - bossComp.ChargeCooldownTime >= bossComp.ChargeInterval))
			{
				// Charge
				if (glm::distance(m_Transform[index]->Position, m_Enemy[index]->Target->Position) <= bossComp.ChargeDistance)
				{
					if (bossComp.ChargeLoadCooldownTime == 0.0f)
					{
						physComp.SpeedMultiplier = 0.0f;
						bossComp.ChargeLoadCooldownTime = Frosty::Time::CurrentTime();
						bossComp.ChargeTargetPosition = m_Enemy[index]->Target->Position;
						physComp.Direction = glm::normalize(bossComp.ChargeTargetPosition - m_Transform[index]->Position);
						LookAtPoint(bossComp.ChargeTargetPosition, index);
						bossComp.ActiveAbility = Frosty::ECS::CBoss::AbilityState::Charge;
					}
				}
			}
		}

		if (bossComp.ActiveAbility == Frosty::ECS::CBoss::AbilityState::Leap)
		{
			return true;
		}
		else if (bossComp.ActiveAbility == Frosty::ECS::CBoss::AbilityState::Charge)
		{
			if (Frosty::Time::CurrentTime() - bossComp.ChargeLoadCooldownTime >= bossComp.ChargeLoadTime && bossComp.ChargeTargetPosition != glm::vec3(0.0f))
			{
				// Loading charge completed
				physComp.SpeedMultiplier = 6.0f;
			}

			bossComp.DistanceCharged += glm::length(physComp.Direction * physComp.Speed * physComp.SpeedMultiplier * Frosty::Time::DeltaTime());
			if (bossComp.DistanceCharged >= bossComp.ChargeDistance)
			{
				bossComp.ActiveAbility = Frosty::ECS::CBoss::AbilityState::None;
				bossComp.ChargeCooldownTime = Frosty::Time::CurrentTime();
				physComp.SpeedMultiplier = 1.0f;
				bossComp.DistanceCharged = 0.0f;
				bossComp.ChargeLoadCooldownTime = 0.0f;
				bossComp.ChargeTargetPosition = glm::vec3(0.0f);
			}



			return true;
		}



		return false;
	}
}