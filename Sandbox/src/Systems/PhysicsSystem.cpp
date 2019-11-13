#include <mcspch.hpp>
#include "PhysicsSystem.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

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
				glm::vec3 offset = Frosty::CollisionDetection::AABBIntersecPushback(finalLengthA, finalCenterA, finalLengthB, finalCenterB);
				if (glm::length(offset) > 0.0f)
				{
					// If collison is an attack...
					if (m_World->HasComponent<Frosty::ECS::CAttack>(m_Transform[index]->EntityPtr))
					{
						auto& comp = m_World->GetComponent<Frosty::ECS::CAttack>(m_Transform[index]->EntityPtr);

						// ... and an enemy has been hit my a player attack --> destroy enemy (should lower HP)
						if (m_World->HasComponent<Frosty::ECS::CEnemy>(m_Transform[i]->EntityPtr) && comp.Friendly)
						{
							if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
							{
								m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
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
						// Destroy attack. The damage has been done
						if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[index]->EntityPtr))
						{
							m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[index]->EntityPtr);
						}
					}
					// If player collides with exit Bounding box
					else if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[index]->EntityPtr) && m_World->HasComponent<Frosty::ECS::CLevelExit>(m_Transform[i]->EntityPtr))
					{
						//to go to next room
						Frosty::EventBus::GetEventBus()->Publish<Frosty::ExitLevelEvent>(Frosty::ExitLevelEvent(m_Transform[i]->EntityPtr, m_Transform[index]->EntityPtr));
					}
					// If the one colliding is an enemy or a player...
					else if (m_World->HasComponent<Frosty::ECS::CEnemy>(m_Transform[index]->EntityPtr) || m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[index]->EntityPtr))
					{
						// ... and it's not colliding with an attack --> back off
						if (!m_World->HasComponent<Frosty::ECS::CAttack>(m_Transform[i]->EntityPtr))
						{
							m_Transform[index]->Position -= offset;
						}
					}
				}
			}
		}
	}
}