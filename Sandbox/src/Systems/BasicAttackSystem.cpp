#include <mcspch.hpp>
#include "BasicAttackSystem.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{

	void BasicAttackSystem::Init()
	{
		m_World = Frosty::Application::Get().GetWorld().get();
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CWeapon>(), true);
	}

	void BasicAttackSystem::OnUpdate()
	{
	}

	void BasicAttackSystem::OnEvent(Frosty::BaseEvent& e)
	{
		switch (e.GetEventType())
		{
		case Frosty::EventType::BasicAttack:
			OnBasicAttackEvent(static_cast<Frosty::BasicAttackEvent&>(e));
			break;
		default:
			break;
		}
	}

	void BasicAttackSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Weapon[p_Total] = &world->GetComponent<Frosty::ECS::CWeapon>(entity);

			p_Total++;
		}
	}

	void BasicAttackSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

		if (tempIndex > 0)
		{
			p_Total--;
			m_Weapon[p_Total] = nullptr;

			if (p_Total > 1)
			{
				//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

				if (p_Total > tempIndex)
				{
					std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Weapon[p_EntityMap[entity]]->EntityPtr;
					p_EntityMap[entityToUpdate] = tempIndex;
				}
			}

			p_EntityMap.erase(entity);
		}
	}

	void BasicAttackSystem::OnBasicAttackEvent(Frosty::BasicAttackEvent& e)
	{
		size_t index = p_EntityMap[e.GetEntity()];

		if (index > 0)
		{
			if (Frosty::Time::CurrentTime() - m_Weapon[index]->CooldownTimer >= m_Weapon[index]->Cooldown)
			{
				switch (m_Weapon[index]->Weapon)
				{
				case Frosty::ECS::CWeapon::WeaponType::Sword:
					CreateBoundingBox(index);
					break;
				case Frosty::ECS::CWeapon::WeaponType::Arrow:
					CreateProjectile(index);
					break;
				default:
					break;
				}

				m_Weapon[index]->CooldownTimer = Frosty::Time::CurrentTime();
			}
		}
		else
		{
			FY_ASSERT(false, "Invalid entity index.");
		}
	}

	void BasicAttackSystem::CreateBoundingBox(size_t index)
	{
		// Get necessary info
		auto& attackerTransform = m_World->GetComponent<Frosty::ECS::CTransform>(m_Weapon[index]->EntityPtr);

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

		m_World->AddComponent<Frosty::ECS::CMesh>(sword, Frosty::AssetManager::GetMesh("pCube1"));				// Should be according to weapon level type in CWeapon
		m_World->AddComponent<Frosty::ECS::CMaterial>(sword, Frosty::AssetManager::GetShader("FlatColor"));
		m_World->AddComponent<Frosty::ECS::CPhysics>(sword, Frosty::AssetManager::GetBoundingBox("pCube1"));

		// Calculate chances of critical hit here //	~ W-_-W ~
		// Is it a friendly attack or an enemy attack? Add boolean in parameter accordingly
		if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Weapon[index]->EntityPtr))
		{
			m_World->AddComponent<Frosty::ECS::CAttack>(sword, Frosty::ECS::CAttack::AttackType::Melee, m_Weapon[index]->Damage + m_Weapon[index]->CriticalHit, true);
		}
		else
		{
			m_World->AddComponent<Frosty::ECS::CAttack>(sword, Frosty::ECS::CAttack::AttackType::Melee, m_Weapon[index]->Damage + m_Weapon[index]->CriticalHit, false);
		}

	}

	void BasicAttackSystem::CreateProjectile(size_t index)
	{
		// Get necessary info
		auto& attackerTransform = m_World->GetComponent<Frosty::ECS::CTransform>(m_Weapon[index]->EntityPtr);

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

		// Calculate chances of critical hit here //	~ W-_-W ~
		if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Weapon[index]->EntityPtr))
		{
			m_World->AddComponent<Frosty::ECS::CAttack>(projectile, Frosty::ECS::CAttack::AttackType::Range, m_Weapon[index]->Damage + m_Weapon[index]->CriticalHit, true, m_Weapon[index]->Lifetime);
		}
		else
		{
			m_World->AddComponent<Frosty::ECS::CAttack>(projectile, Frosty::ECS::CAttack::AttackType::Range, m_Weapon[index]->Damage + m_Weapon[index]->CriticalHit, false, m_Weapon[index]->Lifetime);
		}
	}
}