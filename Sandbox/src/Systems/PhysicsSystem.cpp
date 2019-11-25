#include <mcspch.hpp>
#include "PhysicsSystem.hpp"
#include "Frosty/Events/AbilityEvent.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include"Frosty/API/AssetManager/AssetFiles/BoolMap.hpp"

namespace MCS
{
	const std::string PhysicsSystem::NAME = "Physics";

	void PhysicsSystem::Init()
	{
		m_World = Frosty::Application::Get().GetWorld().get();

		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CPhysics>(), true);
	}

	void PhysicsSystem::OnUpdate()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			m_Transform[i]->Position += m_Physics[i]->Velocity * Frosty::Time::DeltaTime();
			CheckCollision(i);
		}
	}

	void PhysicsSystem::OnEvent(Frosty::BaseEvent& e)
	{

		switch (e.GetEventType())
		{
		case Frosty::EventType::LoadBoolMap:
			OnLoadBoolMapEvent(static_cast<Frosty::BoolMapLoadedEvent&>(e));
			break;
		case Frosty::EventType::UpdateCurrentRoom:
			OnUpdateCurrentRoomEvent(static_cast<Frosty::UpdateCurrentRoomEvent&>(e));
			break;
		}

	}

	void PhysicsSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Physics[p_Total] = &world->GetComponent<Frosty::ECS::CPhysics>(entity);

			p_Total++;
		}
	}

	void PhysicsSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_Physics[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void PhysicsSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CPhysics* physicsPtr = world->GetComponentAddress<Frosty::ECS::CPhysics>(entity);

			m_Transform[it->second] = transformPtr;
			m_Physics[it->second] = physicsPtr;
		}
	}

	std::string PhysicsSystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_Physics[i] << "\t" << m_Physics[i]->EntityPtr->Id << "\t\t" << m_Physics[i]->EntityPtr << "\t\t" << m_Physics[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}

	void PhysicsSystem::OnLoadBoolMapEvent(Frosty::BoolMapLoadedEvent& e)
	{
		int o = 0;
	}

	void PhysicsSystem::OnUpdateCurrentRoomEvent(Frosty::UpdateCurrentRoomEvent& e)
	{
		m_CurrentActiveBoolMap = Frosty::AssetManager::GetBoolMap(e.GetCurrentRoom());
	}

	void PhysicsSystem::CheckCollision(size_t index)
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (index != i && m_Physics[index]->Speed != 0)
			{
				glm::vec3 finalCenterA = m_Transform[index]->Position + glm::vec3(m_Physics[index]->BoundingBox->pos[0], m_Physics[index]->BoundingBox->pos[1], m_Physics[index]->BoundingBox->pos[2]);
				glm::vec3 finalCenterB = m_Transform[i]->Position + glm::vec3(m_Physics[i]->BoundingBox->pos[0], m_Physics[i]->BoundingBox->pos[1], m_Physics[i]->BoundingBox->pos[2]);
				glm::vec3 finalLengthA = glm::vec3(m_Physics[index]->BoundingBox->halfSize[0], m_Physics[index]->BoundingBox->halfSize[1], m_Physics[index]->BoundingBox->halfSize[2]) * m_Transform[index]->Scale;
				glm::vec3 finalLengthB = glm::vec3(m_Physics[i]->BoundingBox->halfSize[0], m_Physics[i]->BoundingBox->halfSize[1], m_Physics[i]->BoundingBox->halfSize[2]) * m_Transform[i]->Scale;
				bool intersect = Frosty::CollisionDetection::AABBIntersect(finalLengthA, finalCenterA, finalLengthB, finalCenterB);

				if (intersect == true)
				{
					// If collison is an attack...
					if (m_World->HasComponent<Frosty::ECS::CAttack>(m_Transform[index]->EntityPtr))
					{
						auto& comp = m_World->GetComponent<Frosty::ECS::CAttack>(m_Transform[index]->EntityPtr);

						// ... and an enemy has been hit my a player attack --> destroy enemy (should lower HP)
						if (m_World->HasComponent<Frosty::ECS::CEnemy>(m_Transform[i]->EntityPtr) && comp.Friendly)
						{
							if (m_World->HasComponent<Frosty::ECS::CDropItem>(m_Transform[i]->EntityPtr))
							{
								SpawnItem(i);
								if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
								{
									m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
								}
							}
							else
							{
								if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
								{
									m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
								}
							}
						}
						// ... and an chest has been hit by a player attack --> destroy Chest 
						else if (m_World->HasComponent<Frosty::ECS::CDropItem>(m_Transform[i]->EntityPtr) && comp.Friendly)
						{
							if (!m_World->HasComponent<Frosty::ECS::CEnemy>(m_Transform[i]->EntityPtr))
							{
								SpawnItem(i);
								if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
								{
									m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
								}
							}
						}
						// ... and a player has been hit by an enemy attack --> destroy player (should lower HP)
						else if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr) && !comp.Friendly)
						{
							if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
							{
								m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
							}
						}
						// Now a check for the actual attack
						if (!(m_World->HasComponent<Frosty::ECS::CAttack>(m_Transform[i]->EntityPtr)))
						{
							// If an undestructible attack collides with a static obj --> make it destroyable
							if (m_World->GetComponent<Frosty::ECS::CTransform>(m_Transform[i]->EntityPtr).IsStatic)
							{
								m_World->GetComponent<Frosty::ECS::CAttack>(m_Transform[index]->EntityPtr).Destroyable = true;
							}
							// Destroy attack. The damage has been done
							if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[index]->EntityPtr) && (m_World->GetComponent<Frosty::ECS::CAttack>(m_Transform[index]->EntityPtr).Destroyable))
							{
								m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[index]->EntityPtr);
							}
						}
					}
					// If player collides with exit Bounding box
					else if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[index]->EntityPtr) && m_World->HasComponent<Frosty::ECS::CLevelExit>(m_Transform[i]->EntityPtr))
					{
						//to go to next room
						auto& Exit = m_World->GetComponent<Frosty::ECS::CLevelExit>(m_Transform[i]->EntityPtr);
						// GetCurrentRoom 
						//Load To other
						if (!Exit.IsTriggered)
						{
							glm::ivec2 CoordsTest = m_World->GetComponent<Frosty::ECS::CLevelExit>(m_Transform[i]->EntityPtr).RoomCoords;

							Exit.IsTriggered = true;
							Frosty::EventBus::GetEventBus()->Publish
							<Frosty::UpdatePlayerRoomCoordEvent>(Frosty::UpdatePlayerRoomCoordEvent
							(m_World->GetComponent<Frosty::ECS::CLevelExit>(m_Transform[i]->EntityPtr).RoomCoords));

							Frosty::EventBus::GetEventBus()->Publish
							<Frosty::ExitLevelEvent>(Frosty::ExitLevelEvent
							(m_Transform[i]->EntityPtr, m_Transform[index]->EntityPtr));
						}
						//else
						//{
						//		Frosty::EventBus::GetEventBus()->Publish
						//		<Frosty::UpdatePlayerRoomCoordEvent>(Frosty::UpdatePlayerRoomCoordEvent
						//		(m_World->GetComponent<Frosty::ECS::CLevelExit>(m_Transform[i]->EntityPtr).RoomCoords));
						//}
					}
					// If the one colliding is an enemy or a player...
					else if (m_World->HasComponent<Frosty::ECS::CEnemy>(m_Transform[index]->EntityPtr) || m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[index]->EntityPtr))
					{
						// ... and it's not colliding with an attack --> back off
						if (!m_World->HasComponent<Frosty::ECS::CAttack>(m_Transform[i]->EntityPtr))
						{
							m_Transform[index]->Position -= Frosty::CollisionDetection::AABBIntersecPushback(finalLengthA, finalCenterA, finalLengthB, finalCenterB);
						}
					}
				
					
				}
				else if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[index]->EntityPtr) && m_World->HasComponent<Frosty::ECS::CLevelExit>(m_Transform[i]->EntityPtr))
				{
					auto& Exit = m_World->GetComponent<Frosty::ECS::CLevelExit>(m_Transform[i]->EntityPtr);
					Exit.IsTriggered = false;
				}
			}
		}
	}

	void PhysicsSystem::SpawnItem(size_t index)
	{
		m_RandItem = (rand() % 6) + 1;

		auto& item = m_World->CreateEntity({ m_Transform.at(index)->Position }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f });
		m_World->AddComponent<Frosty::ECS::CMesh>(item, Frosty::AssetManager::GetMesh("pCube1"));
		m_World->AddComponent<Frosty::ECS::CMaterial>(item, Frosty::AssetManager::GetShader("FlatColor"));
		m_World->AddComponent<Frosty::ECS::CPhysics>(item, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);

		//add wolfsbain and bait as items to be dropt by chest!
		switch (m_RandItem)
		{
		case 1:
			m_World->AddComponent<Frosty::ECS::CLootable>(item, Frosty::ECS::CLootable::LootType::HealingPotion);
			break;
		case 2:
			m_World->AddComponent<Frosty::ECS::CLootable>(item, Frosty::ECS::CLootable::LootType::IncHealthPotion);
			break;
		case 3:
			m_World->AddComponent<Frosty::ECS::CLootable>(item, Frosty::ECS::CLootable::LootType::SpeedPotion);
			break;
		case 4:
			m_World->AddComponent<Frosty::ECS::CLootable>(item, Frosty::ECS::CLootable::LootType::SpeedBoot);
			break;
		case 5:
			m_World->AddComponent<Frosty::ECS::CLootable>(item, Frosty::ECS::CLootable::LootType::Sword1);
			m_World->AddComponent<Frosty::ECS::CWeapon>(item, Frosty::ECS::CWeapon::WeaponType::Sword, 1, 1.f);
			break;
		case 6:
			m_World->AddComponent<Frosty::ECS::CLootable>(item, Frosty::ECS::CLootable::LootType::Arrow1);
			m_World->AddComponent<Frosty::ECS::CWeapon>(item, Frosty::ECS::CWeapon::WeaponType::Bow, 1, 1.f);
			break;
		default:
			break;
		}
	}
}
