#include <mcspch.hpp>
#include "PhysicsSystem.hpp"
#include "Frosty/Events/AbilityEvent.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include"Frosty/Core/BoolMap/BoolMap.hpp"

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
			//if (i == 2 && Frosty::Time::GetFrameCount() % 100 == 0) FY_INFO("Direction is ({0}, {1}, {2})", m_Physics[i]->Direction.x, m_Physics[i]->Direction.y, m_Physics[i]->Direction.z);
			//if (i == 2 && Frosty::Time::GetFrameCount() % 10 == 0) FY_INFO("Speed multiplier is ({0})", m_Physics[i]->SpeedMultiplier);

			glm::vec3 changeOffset = m_Physics[i]->Direction * m_Physics[i]->Speed * m_Physics[i]->SpeedMultiplier * Frosty::Time::DeltaTime();
			m_Transform[i]->Position += changeOffset;
			if (!m_Transform[i]->IsStatic) CheckCollision(i);

			if (m_Physics[i]->Direction.y > 0.0f)
			{
				m_Physics[i]->HangTime -= Frosty::Time::DeltaTime();

				if (m_Physics[i]->HangTime <= 0.0f)
				{
					m_Physics[i]->HangTime = 0.0f;
					m_Physics[i]->Direction = glm::vec3(0.0f, -1.0f, 0.0f);
				}
			}
			else if (m_Physics[i]->Direction.y < 0.0f && m_Transform[i]->Position.y < 0.0f)
			{
				m_Physics[i]->SpeedMultiplier = 1.0f;
				m_Physics[i]->Direction = glm::vec3(0.0f);
			}
		}
	}

	void PhysicsSystem::OnEvent(Frosty::BaseEvent& e)
	{

		switch (e.GetEventType())
		{
		case Frosty::EventType::LoadBoolMap:
			OnLoadBoolMapEvent(static_cast<Frosty::BoolMapLoadedEvent&>(e));
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

	void PhysicsSystem::CheckCollision(size_t index)
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (index != i)
			{
				bool checkCollision = false;
				// Check all collision types that you wanna ignore
				if (m_World->HasComponent<Frosty::ECS::CAttack>(m_Transform[index]->EntityPtr) &&
					(m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr) || m_World->HasComponent<Frosty::ECS::CDropItem>(m_Transform[i]->EntityPtr)))
				{
					checkCollision = true;
				}
				else if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[index]->EntityPtr) && (m_World->HasComponent<Frosty::ECS::CLevelExit>(m_Transform[i]->EntityPtr) ||
					m_Transform[i]->IsStatic || m_World->HasComponent<Frosty::ECS::CDropItem>(m_Transform[i]->EntityPtr)))		// Add witch circle check here
				{
					checkCollision = true;
				}
				else if (m_World->HasComponent<Frosty::ECS::CEnemy>(m_Transform[index]->EntityPtr) && (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr) || m_Transform[i]->IsStatic))
				{
					checkCollision = true;
				}

				if (checkCollision)
				{
					
					glm::vec3 finalCenterA = m_Transform[index]->Position + glm::vec3(m_Physics[index]->BoundingBox->pos[0], m_Physics[index]->BoundingBox->pos[1], m_Physics[index]->BoundingBox->pos[2]);
					glm::vec3 finalCenterB = m_Transform[i]->Position + glm::vec3(m_Physics[i]->BoundingBox->pos[0], m_Physics[i]->BoundingBox->pos[1], m_Physics[i]->BoundingBox->pos[2]);
					glm::vec3 finalLengthA = glm::vec3(m_Physics[index]->BoundingBox->halfSize[0], m_Physics[index]->BoundingBox->halfSize[1], m_Physics[index]->BoundingBox->halfSize[2]) * m_Transform[index]->Scale;
					glm::vec3 finalLengthB = glm::vec3(m_Physics[i]->BoundingBox->halfSize[0], m_Physics[i]->BoundingBox->halfSize[1], m_Physics[i]->BoundingBox->halfSize[2]) * m_Transform[i]->Scale;
					bool intersect = Frosty::CollisionDetection::AABBIntersect(finalLengthA, finalCenterA, finalLengthB, finalCenterB);
					glm::vec3 intersectionPushback(0.0f);

					if (!m_Transform[i]->IsStatic) intersectionPushback = CircleIntersection(index, i);

					if ((intersectionPushback != glm::vec3(0.0f, 0.0f, 0.0f)) || intersect)
					{
						// Attack with Player/Enemy/Chest
						if (m_World->HasComponent<Frosty::ECS::CAttack>(m_Transform[index]->EntityPtr))
						{
							auto& attack = m_World->GetComponent<Frosty::ECS::CAttack>(m_Transform[index]->EntityPtr);

							// Attack - Enemy or Player
							if (m_World->HasComponent<Frosty::ECS::CDropItem>(m_Transform[i]->EntityPtr) || m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr))
							{
								if (attack.Friendly && m_World->HasComponent<Frosty::ECS::CDropItem>(m_Transform[i]->EntityPtr))
								{
									// Player Attack - Enemy or Chest
									if (!m_World->HasComponent<Frosty::ECS::CEnemy>(m_Transform[i]->EntityPtr))
									{
										Frosty::EventBus::GetEventBus()->Publish<Frosty::DropItemEvent>(Frosty::DropItemEvent(m_Transform[i]->EntityPtr));

										if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
										{
											m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
										}
										Frosty::EventBus::GetEventBus()->Publish<Frosty::EnemyDeathEvent>(Frosty::EnemyDeathEvent(30));
									}
									else
									{
										Frosty::EventBus::GetEventBus()->Publish<Frosty::CollisionEvent>(Frosty::CollisionEvent(m_Transform[index]->EntityPtr, m_Transform[i]->EntityPtr));
									}
								}
								else if (!attack.Friendly && m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr))
								{
									// Enemy Attack - Player
									Frosty::EventBus::GetEventBus()->Publish<Frosty::CollisionEvent>(Frosty::CollisionEvent(m_Transform[index]->EntityPtr, m_Transform[i]->EntityPtr));
								}
							}
						}
						// Player with Exit Level
						else if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[index]->EntityPtr) && m_World->HasComponent<Frosty::ECS::CLevelExit>(m_Transform[i]->EntityPtr))
						{
							Frosty::EventBus::GetEventBus()->Publish<Frosty::ExitLevelEvent>(Frosty::ExitLevelEvent(m_Transform[i]->EntityPtr, m_Transform[index]->EntityPtr));
						}
						// Enemy or Player colliding
						else if (m_World->HasComponent<Frosty::ECS::CEnemy>(m_Transform[index]->EntityPtr) || m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[index]->EntityPtr))
						{
							bool normalCollisionPushback = true;
							if (m_World->HasComponent<Frosty::ECS::CBoss>(m_Transform[index]->EntityPtr))
							{
								auto& bossComp = m_World->GetComponent<Frosty::ECS::CBoss>(m_Transform[index]->EntityPtr);

								if (bossComp.ActiveAbility == Frosty::ECS::CBoss::AbilityState::Leap)
								{
									if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr))
									{
										// Leap colliding into player
										m_Physics[index]->SpeedMultiplier = 1.0f;
										bossComp.LeapCooldownTime = Frosty::Time::CurrentTime();
										bossComp.ActiveAbility = Frosty::ECS::CBoss::AbilityState::None;
										bossComp.LeapTargetPosition = glm::vec3(0.0f);
										Frosty::EventBus::GetEventBus()->Publish<Frosty::DamageEvent>(Frosty::DamageEvent(m_Transform[i]->EntityPtr, bossComp.LeapDamage));
									}
									bool normalCollisionPushback = false;
								}
								else if (bossComp.ActiveAbility == Frosty::ECS::CBoss::AbilityState::Charge)
								{
									if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr))
									{
										// Charge into a player, push player back
										// Make the player to float up
										m_Physics[i]->Direction = glm::vec3(0.0f, 1.0f, 0.0f);
										m_Physics[i]->HangTime = bossComp.ChargeHangTime;
										m_Physics[i]->SpeedMultiplier = 0.5f;

										// Reset charge attributes
										bossComp.DistanceCharged = bossComp.ChargeDistance;
										m_Physics[index]->SpeedMultiplier = 1.0f;
										bossComp.ActiveAbility = Frosty::ECS::CBoss::AbilityState::None;
										bossComp.ChargeTargetPosition = glm::vec3(0.0f);
										bossComp.ChargeLoadCooldownTime = 0.0f;
										normalCollisionPushback = false;
										Frosty::EventBus::GetEventBus()->Publish<Frosty::DamageEvent>(Frosty::DamageEvent(m_Transform[i]->EntityPtr, bossComp.ChargeDamage));
									}
									else if (m_World->HasComponent<Frosty::ECS::CEnemy>(m_Transform[i]->EntityPtr))
									{
										normalCollisionPushback = false;
									}
									else if (m_Transform[i]->IsStatic)
									{
										// Charge into a static obstacle
										bossComp.DistanceCharged = bossComp.ChargeDistance;
										m_Physics[index]->SpeedMultiplier = 1.0f;
										bossComp.ActiveAbility = Frosty::ECS::CBoss::AbilityState::None;
										bossComp.ChargeTargetPosition = glm::vec3(0.0f);
										bossComp.ChargeLoadCooldownTime = 0.0f;
									}
								}
							}
							

							if (m_Transform[i]->IsStatic)
							{
								if (normalCollisionPushback && intersect)
									m_Transform[index]->Position -= Frosty::CollisionDetection::AABBIntersecPushback(finalLengthA, finalCenterA, finalLengthB, finalCenterB);
							}
							else
							{
								if (normalCollisionPushback) m_Transform[index]->Position -= intersectionPushback;
							}
						}
					}
				}


				//{
				//	// If collision is an attack...
				//	if (m_World->HasComponent<Frosty::ECS::CAttack>(m_Transform[index]->EntityPtr))
				//	{
				//		auto& comp = m_World->GetComponent<Frosty::ECS::CAttack>(m_Transform[index]->EntityPtr);
				//
				//		// ... and an enemy has been hit my a player attack --> destroy enemy (should lower HP)
				//		if (m_World->HasComponent<Frosty::ECS::CEnemy>(m_Transform[i]->EntityPtr) && comp.Friendly)
				//		{
				//			if (m_World->HasComponent<Frosty::ECS::CDropItem>(m_Transform[i]->EntityPtr))
				//			{
				//				SpawnItem(i);
				//				if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
				//				{
				//					m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
				//				}
				//			}
				//			else
				//			{
				//				if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
				//				{
				//					m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
				//				}
				//			}
				//		}
				//		// ... and an chest has been hit by a player attack --> destroy Chest 
				//		else if (m_World->HasComponent<Frosty::ECS::CDropItem>(m_Transform[i]->EntityPtr) && comp.Friendly)
				//		{
				//			if (!m_World->HasComponent<Frosty::ECS::CEnemy>(m_Transform[i]->EntityPtr))
				//			{
				//				SpawnItem(i);
				//				if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
				//				{
				//					m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
				//				}
				//			}
				//		}
				//		// ... and a player has been hit by an enemy attack --> destroy player (should lower HP)
				//		else if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr) && !comp.Friendly)
				//		{
				//			if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
				//			{
				//				m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
				//			}
				//		}
				//		// Now a check for the actual attack
				//		if (!(m_World->HasComponent<Frosty::ECS::CAttack>(m_Transform[i]->EntityPtr)))
				//		{
				//			// If an undestructible attack collides with a static obj --> make it destroyable
				//			if (m_World->GetComponent<Frosty::ECS::CTransform>(m_Transform[i]->EntityPtr).IsStatic)
				//			{
				//				m_World->GetComponent<Frosty::ECS::CAttack>(m_Transform[index]->EntityPtr).Destroyable = true;
				//			}
				//			// Destroy attack. The damage has been done
				//			if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[index]->EntityPtr) && (m_World->GetComponent<Frosty::ECS::CAttack>(m_Transform[index]->EntityPtr).Destroyable))
				//			{
				//				m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[index]->EntityPtr);
				//			}
				//		}
				//	}
				//	// If player collides with exit Bounding box
				//	else if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[index]->EntityPtr) && m_World->HasComponent<Frosty::ECS::CLevelExit>(m_Transform[i]->EntityPtr))
				//	{
				//		//to go to next room
				//		Frosty::EventBus::GetEventBus()->Publish<Frosty::ExitLevelEvent>(Frosty::ExitLevelEvent(m_Transform[i]->EntityPtr, m_Transform[index]->EntityPtr));
				//	}
				//	// If the one colliding is an enemy or a player...
				//	else if (m_World->HasComponent<Frosty::ECS::CEnemy>(m_Transform[index]->EntityPtr) || m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[index]->EntityPtr))
				//	{
				//	// ... and it's not colliding with an attack --> back off
				//	if (!m_World->HasComponent<Frosty::ECS::CAttack>(m_Transform[i]->EntityPtr) && !m_World->HasComponent<Frosty::ECS::CWitchCircle>(m_Transform[i]->EntityPtr))
				//	{
				//		m_Transform[index]->Position -= Frosty::CollisionDetection::AABBIntersecPushback(finalLengthA, finalCenterA, finalLengthB, finalCenterB);
				//	}
				//}
			}
		}
	}

	glm::vec3 PhysicsSystem::CircleIntersection(size_t indexA, size_t indexB)
	{
		float dist = glm::distance(m_Transform[indexA]->Position, m_Transform[indexB]->Position);
		float totalRadius = m_Physics[indexA]->Radius + m_Physics[indexB]->Radius;
		float diff = dist - totalRadius;
		//if (indexA == 1 && Frosty::Time::GetFrameCount() % 60 == 0)
		//{
		//	FY_INFO("Distance: {0}", dist);
		//	FY_INFO("Total Radius: {0}", totalRadius);
		//	FY_INFO("Difference: {0}", diff);
		//}
		if (diff <= 0.0f)
			return m_Physics[indexA]->Direction * diff * -1.0f;

		return glm::vec3(0.0f);
	}
}
