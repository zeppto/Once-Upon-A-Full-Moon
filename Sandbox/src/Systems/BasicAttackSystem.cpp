#include <mcspch.hpp>
#include "BasicAttackSystem.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{
	const std::string BasicAttackSystem::NAME = "Basic Attack";

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
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_BasicAttack[p_Total]->EntityPtr;
			m_BasicAttack[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void BasicAttackSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CBasicAttack* basicAttackPtr = world->GetComponentAddress<Frosty::ECS::CBasicAttack>(entity);

			m_BasicAttack[it->second] = basicAttackPtr;
		}
	}

	std::string BasicAttackSystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_BasicAttack[i] << "\t" << m_BasicAttack[i]->EntityPtr->Id << "\t\t" << m_BasicAttack[i]->EntityPtr << "\t\t" << m_BasicAttack[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}

	void BasicAttackSystem::OnBasicAttackEvent(Frosty::BasicAttackEvent& e)
	{
		auto& it = p_EntityMap.find(e.GetEntity());

		if (it != p_EntityMap.end())
		{
			if (m_BasicAttack[it->second]->CurrentCooldown <= 0.0f)
			{
				switch (m_BasicAttack[it->second]->Type)
				{
				case Frosty::ECS::CBasicAttack::AttackType::Melee:
					CreateBoundingBox(it->second);
					break;
				case Frosty::ECS::CBasicAttack::AttackType::Range:
					CreateProjectile(it->second);
					break;
				default:
					break;
				}

				m_BasicAttack[it->second]->CurrentCooldown = m_BasicAttack[it->second]->COOLDOWN / 1000.0f;
			}
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
		world->AddComponent<Frosty::ECS::CMesh>(projectile, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(projectile, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(projectile, Frosty::AssetManager::GetBoundingBox("pCube1"));
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
		world->AddComponent<Frosty::ECS::CMesh>(projectile, Frosty::AssetManager::GetMesh("pSphere1"));
		world->AddComponent<Frosty::ECS::CMaterial>(projectile, Frosty::AssetManager::GetShader("FlatColor"));
		auto& projectilePhysics = world->AddComponent<Frosty::ECS::CPhysics>(projectile, Frosty::AssetManager::GetBoundingBox("pSphere1"), 20.0f);
		projectilePhysics.Velocity = direction * projectilePhysics.Speed;
		world->AddComponent<Frosty::ECS::CArrow>(projectile);
	}

}