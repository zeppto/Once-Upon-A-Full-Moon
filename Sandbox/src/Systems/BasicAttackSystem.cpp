#include <mcspch.hpp>
#include "BasicAttackSystem.hpp"
#include "Frosty/API/AssetManager/AM.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{

	void BasicAttackSystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CBasicAttack>(), true);
	}

	void BasicAttackSystem::OnUpdate()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_BasicAttack[i]->CurrentCooldown > 0.0) m_BasicAttack[i]->CurrentCooldown -= Frosty::Time::DeltaTime();
		}
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
			m_BasicAttack[p_Total] = &world->GetComponent<Frosty::ECS::CBasicAttack>(entity);

			p_Total++;
		}
	}

	void BasicAttackSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

		if (tempIndex > 0)
		{
			p_Total--;
			m_BasicAttack[p_Total] = nullptr;

			if (p_Total > 1)
			{
				//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

				if (p_Total > tempIndex)
				{
					std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_BasicAttack[p_EntityMap[entity]]->EntityPtr;
					p_EntityMap[entityToUpdate] = tempIndex;
				}
			}

			p_EntityMap.erase(entity);
		}
	}

	void BasicAttackSystem::OnBasicAttackEvent(Frosty::BasicAttackEvent& e)
	{
		size_t index = p_EntityMap[e.GetEntity()];

		if (m_BasicAttack[index]->CurrentCooldown <= 0.0f)
		{
			switch (m_BasicAttack[index]->Type)
			{
			case Frosty::ECS::CBasicAttack::AttackType::Melee:
				CreateBoundingBox(index);
				break;
			case Frosty::ECS::CBasicAttack::AttackType::Range:
				CreateProjectile(index);
				break;
			default:
				break;
			}

			m_BasicAttack[index]->CurrentCooldown = m_BasicAttack[index]->COOLDOWN / 1000.0f;
		}
	}

	void BasicAttackSystem::CreateBoundingBox(size_t index)
	{
		// Get necessary info
		auto& world = Frosty::Application::Get().GetWorld();
		auto& attackerTransform = world->GetComponent<Frosty::ECS::CTransform>(m_BasicAttack[index]->EntityPtr);

		// Calculate direction vector
		glm::mat4 mat = glm::mat4(1.0f);
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.x), { 1.0f, 0.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.y), { 0.0f, 1.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.z), { 0.0f, 0.0f, 1.0f });
		glm::vec4 direction = mat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0);

		// Create BB
		glm::vec3 spawnPos = attackerTransform.Position + (glm::vec3(direction) * 4.0f);
		glm::vec3 spawnScale = glm::vec3(10.0f, 6.0f, 4.0f);
		auto& projectile = world->CreateEntity({ spawnPos.x, 3.0f, spawnPos.z }, attackerTransform.Rotation, spawnScale);
		world->AddComponent<Frosty::ECS::CMesh>(projectile, Frosty::AssetManager::GetMesh("Cube"));
		world->AddComponent<Frosty::ECS::CMaterial>(projectile, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(projectile, Frosty::AssetManager::GetBoundingBox("Cube"));
		world->AddComponent<Frosty::ECS::CSword>(projectile);
	}

	void BasicAttackSystem::CreateProjectile(size_t index)
	{
		// Get necessary info
		auto& world = Frosty::Application::Get().GetWorld();
		auto& attackerTransform = world->GetComponent<Frosty::ECS::CTransform>(m_BasicAttack[index]->EntityPtr);

		// Calculate direction vector
		glm::mat4 mat = glm::mat4(1.0f);
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.x), { 1.0f, 0.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.y), { 0.0f, 1.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(attackerTransform.Rotation.z), { 0.0f, 0.0f, 1.0f });
		glm::vec4 direction = mat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0);

		// Create projectile
		glm::vec3 spawnPos = attackerTransform.Position + (glm::vec3(direction) * 3.0f);
		auto& projectile = world->CreateEntity({ spawnPos.x, 1.0f, spawnPos.z }, attackerTransform.Rotation, { 0.3f, 0.3f, 0.3f });
		world->AddComponent<Frosty::ECS::CMesh>(projectile, Frosty::AssetManager::GetMesh("Sphere"));
		world->AddComponent<Frosty::ECS::CMaterial>(projectile, Frosty::AssetManager::GetShader("FlatColor"));
		auto& projectilePhysics = world->AddComponent<Frosty::ECS::CPhysics>(projectile, Frosty::AssetManager::GetBoundingBox("Sphere"), 20.0f);
		projectilePhysics.Velocity = direction * projectilePhysics.Speed;
		world->AddComponent<Frosty::ECS::CArrow>(projectile);
	}

}