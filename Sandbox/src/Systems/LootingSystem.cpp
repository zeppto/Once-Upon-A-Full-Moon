#include <mcspch.hpp>
#include "LootingSystem.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{
	const std::string LootingSystem::NAME = "Looting";

	void LootingSystem::Init()
	{
		m_World = Frosty::Application::Get().GetWorld().get();
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CLootable>(), true);
	}

	void LootingSystem::OnUpdate()
	{
		m_CoolDown += Frosty::Time::DeltaTime();
	}

	void LootingSystem::OnEvent(Frosty::BaseEvent & e)
	{
		switch (e.GetEventType())
		{
		case Frosty::EventType::PickUpAttempt:
			OnPickUpAttemptEvent(static_cast<Frosty::PickUpAttemptEvent&>(e));
			break;
		case Frosty::EventType::EnterNewRoom:
			OnEnterNewRoom(static_cast<Frosty::EnterNewRoomEvent&>(e));
			break;
		case Frosty::EventType::DropItem:
			OnDropItemEvent(static_cast<Frosty::DropItemEvent&>(e));
			break;
		default:
			break;
		}
	}

	void LootingSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			m_Transform[p_Total] = &m_World->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Lootable[p_Total] = &m_World->GetComponent<Frosty::ECS::CLootable>(entity);
			p_Total++;
		}
	}

	void LootingSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Lootable[p_Total] = nullptr;
			m_Transform[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void LootingSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			Frosty::ECS::CTransform* transformPtr = m_World->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CLootable* lootPtr = m_World->GetComponentAddress<Frosty::ECS::CLootable>(entity);

			m_Transform[it->second] = transformPtr;
			m_Lootable[it->second] = lootPtr;
		}
	}

	std::string LootingSystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_Lootable[i] << "\t" << m_Lootable[i]->EntityPtr->Id << "\t\t" << m_Lootable[i]->EntityPtr << "\t\t" << m_Lootable[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}

	void LootingSystem::OnPickUpAttemptEvent(Frosty::PickUpAttemptEvent & e)
	{
		m_Player = e.GetEntity();

		for (size_t i = 1; i < p_Total; i++)
		{
			float distanceToItem = Distance2D(m_Transform[i]->Position, m_World->GetComponent<Frosty::ECS::CTransform>(m_Player).Position);

			if (distanceToItem < m_WorkingArea && m_CoolDown > 2.0f)
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::PickUpEvent>(Frosty::PickUpEvent(m_Lootable[i]->EntityPtr));
				FY_INFO("{0}", distanceToItem);
			}
		}
	}

	float LootingSystem::Distance2D(glm::vec3 pos1, glm::vec3 pos2)
	{
		glm::vec2 position1;
		position1.x = pos1.x;
		position1.y = pos1.z;

		glm::vec2 position2;
		position2.x = pos2.x;
		position2.y = pos2.z;

		return glm::distance(position1, position2);
	}
	
	void LootingSystem::OnEnterNewRoom(Frosty::EnterNewRoomEvent& e)
	{
		m_TotalRoomsVisited = e.GetTotalvisitedRooms();
	}
	
	void LootingSystem::OnDropItemEvent(Frosty::DropItemEvent& e)
	{
		if (m_World->HasComponent<Frosty::ECS::CDropItem>(e.GetEntity()))
		{
			auto& entityTransform = m_World->GetComponent<Frosty::ECS::CTransform>(e.GetEntity());
			int randomValue;

			if (m_World->HasComponent<Frosty::ECS::CEnemy>(e.GetEntity()))
			{
				randomValue = (rand() % 7) % 5;
				int dropItemChanse = rand() % 2;
				if (dropItemChanse == 0)
					randomValue = -1;
			}
			else
				randomValue = (rand() % 4) + 3;

			if (randomValue != -1)
			{

				Frosty::Weapon weapon;
				auto& item = m_World->CreateEntity(entityTransform.Position + glm::vec3(0.0f, 0.3f, 0.0f), { 0.0f, 0.0f, 0.0f }, { 3.f, 3.f, 3.f });
				m_World->AddToGroup(item, true);
				auto& transform = m_World->GetComponent<Frosty::ECS::CTransform>(item);
				auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(item, Frosty::AssetManager::GetShader("Texture2D"), true);
				auto& light = m_World->AddComponent<Frosty::ECS::CLight>(item, Frosty::ECS::CLight::LightType::Point, 3.f, glm::vec3(1.f, 1.f, 1.f), 2.f);
				auto& loot = m_World->AddComponent<Frosty::ECS::CLootable>(item);

				switch (randomValue)
				{
				case 0:
					// Healing Position
					m_World->AddComponent<Frosty::ECS::CMesh>(item, Frosty::AssetManager::GetMesh("pPlane1"));
					material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("hpPotion");
					light.Color = glm::vec3(1.f, 0.f, 0.f);
					loot.Type = Frosty::ECS::CLootable::LootType::HealingPotion;
					break;
				case 1:
					// Speed Potion
					m_World->AddComponent<Frosty::ECS::CMesh>(item, Frosty::AssetManager::GetMesh("pPlane1"));
					material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("spPotion");
					light.Color = glm::vec3(0.f, 0.f, 1.f);
					loot.Type = Frosty::ECS::CLootable::LootType::SpeedPotion;
					break;
				case 2:
					// Wolfsbane
					m_World->AddComponent<Frosty::ECS::CMesh>(item, Frosty::AssetManager::GetMesh("pPlane1"));
					material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("wolfsbane");
					light.Color = glm::vec3(0.8f, 0.f, 1.f);
					loot.Type = Frosty::ECS::CLootable::LootType::Wolfsbane;
					break;
				case 3:
					// Speed Boots
					transform.Scale = glm::vec3(5.5f, 7.f, 5.5f);
					m_World->AddComponent<Frosty::ECS::CMesh>(item, Frosty::AssetManager::GetMesh("pPlane1"));
					material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("speedBoots");
					light.Color = glm::vec3(0.f, 1.f, 0.f);
					loot.Type = Frosty::ECS::CLootable::LootType::SpeedBoots;
					break;
				case 4:
					// Increase Health Potion
					m_World->AddComponent<Frosty::ECS::CMesh>(item, Frosty::AssetManager::GetMesh("pPlane1"));
					material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("highlightHeart");
					light.Color = glm::vec3(1.f, 0.f, 0.f);
					loot.Type = Frosty::ECS::CLootable::LootType::IncHealthPotion;
					break;
				case 5:
					// Weapon
					transform.Rotation = glm::vec3(130.f, 90.f, 0.f);
					light.Color = glm::vec3(1.f, 1.f, 0.5f);
					light.Radius = 5.f;
					loot.Type = Frosty::ECS::CLootable::LootType::Weapon;

					weapon = GenerateWeapon();
					m_World->AddComponent<Frosty::ECS::CWeapon>(item, weapon);

					if (weapon.Type == Frosty::Weapon::WeaponType::Sword)
					{
						m_World->AddComponent<Frosty::ECS::CMesh>(item, Frosty::AssetManager::GetMesh("sword"));
						material.NormalTexture = Frosty::AssetManager::GetTexture2D("sword_normal");
						m_World->AddComponent<Frosty::ECS::CPhysics>(item, Frosty::AssetManager::GetBoundingBox("sword"), transform.Scale, weapon.ProjectileSpeed);

						if (weapon.Level == 1)
						{
							loot.Weapon = Frosty::ECS::CLootable::WeaponType::Sword1;
							material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("sword_lvl1_diffuse");
						}
						else if (weapon.Level == 2)
						{
							loot.Weapon = Frosty::ECS::CLootable::WeaponType::Sword2;
							material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("sword_lvl2_diffuse");
						}
						else if (weapon.Level == 3)
						{
							loot.Weapon = Frosty::ECS::CLootable::WeaponType::Sword3;
							material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("sword_lvl3_diffuse");
						}
					}
					else if (weapon.Type == Frosty::Weapon::WeaponType::Bow)
					{
						m_World->AddComponent<Frosty::ECS::CMesh>(item, Frosty::AssetManager::GetMesh("Bow"));
						material.NormalTexture = Frosty::AssetManager::GetTexture2D("bow_normal");
						m_World->AddComponent<Frosty::ECS::CPhysics>(item, Frosty::AssetManager::GetBoundingBox("Bow"), transform.Scale, weapon.ProjectileSpeed);

						if (weapon.Level == 1)
						{
							loot.Weapon = Frosty::ECS::CLootable::WeaponType::Bow1;
							material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("bow_lvl1_diffuse");
						}
						else if (weapon.Level == 2)
						{
							loot.Weapon = Frosty::ECS::CLootable::WeaponType::Bow2;
							material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("bow_lvl2_diffuse");
						}
						else if (weapon.Level == 3)
						{
							loot.Weapon = Frosty::ECS::CLootable::WeaponType::Bow3;
							material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("bow_lvl3_diffuse");
						}
					}

					break;
				case 6:
					// Bait
					transform.Scale = glm::vec3(1.f, 1.f, 1.f);
					m_World->AddComponent<Frosty::ECS::CMesh>(item, Frosty::AssetManager::GetMesh("meat"));
					material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("meat");
					light.Color = glm::vec3(1.f, 0.5f, 1.f);
					loot.Type = Frosty::ECS::CLootable::LootType::Bait;
					break;
				default:
					break;
				}
			}
		}
	}

	const Frosty::Weapon& LootingSystem::GenerateWeapon()
	{
		auto& weaponHandler = Frosty::AssetManager::GetWeaponHandler("Weapons");

		//if (m_TotalRoomsVisited < 3)
		//	return weaponHandler->GetAPlayerWeapon(1, 3);
		//else if (m_TotalRoomsVisited < 6)
		//	return weaponHandler->GetAPlayerWeapon(1, 3);
		//
		return weaponHandler->GetAPlayerWeapon(2, 3);
	}
}