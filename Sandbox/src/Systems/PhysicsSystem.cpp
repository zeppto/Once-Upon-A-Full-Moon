#include <mcspch.hpp>
#include "PhysicsSystem.hpp"

namespace MCS
{
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
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

		if (tempIndex > 0)
		{
			p_Total--;
			m_Transform[p_Total] = nullptr;
			m_Physics[p_Total] = nullptr;

			if (p_Total > 1)
			{
				//	std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

				if (p_Total > tempIndex)
				{
					std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
					p_EntityMap[entityToUpdate] = tempIndex;
				}
			}

			p_EntityMap.erase(entity);
		}
	}
	
	void PhysicsSystem::CheckCollision(size_t index)
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (index != i)
			{
				glm::vec3 finalCenterA = m_Transform[index]->Position + glm::vec3(m_Physics[index]->BoundingBox->pos[0], m_Physics[index]->BoundingBox->pos[1], m_Physics[index]->BoundingBox->pos[2]);
				glm::vec3 finalCenterB = m_Transform[i]->Position + glm::vec3(m_Physics[i]->BoundingBox->pos[0], m_Physics[i]->BoundingBox->pos[1], m_Physics[i]->BoundingBox->pos[2]);
				glm::vec3 finalLengthA = glm::vec3(m_Physics[index]->BoundingBox->halfSize[0], m_Physics[index]->BoundingBox->halfSize[1], m_Physics[index]->BoundingBox->halfSize[2]) * m_Transform[index]->Scale;
				glm::vec3 finalLengthB = glm::vec3(m_Physics[i]->BoundingBox->halfSize[0], m_Physics[i]->BoundingBox->halfSize[1], m_Physics[i]->BoundingBox->halfSize[2]) * m_Transform[i]->Scale;
				glm::vec3 offset = Frosty::CollisionDetection::AABBIntersecPushback(finalLengthA, finalCenterA, finalLengthB, finalCenterB);
				if (glm::length(offset) > 0.0f)
				{
					if (m_World->HasComponent<Frosty::ECS::CArrow>(m_Transform[index]->EntityPtr) || m_World->HasComponent<Frosty::ECS::CSword>(m_Transform[index]->EntityPtr))
					{
						// Attack (Arrow/Sword, change to an Attack component)
						if (m_World->HasComponent<Frosty::ECS::CHealth>(m_Transform[i]->EntityPtr))
						{
							// Enemy (In this case as only enemies has CHealth for now. Later have an Enemy component)
							if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
							{
								m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
							}
						}

						if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[index]->EntityPtr))
						{
							m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[index]->EntityPtr);
						}
					}
					else if (m_World->HasComponent<Frosty::ECS::CHealth>(m_Transform[index]->EntityPtr) || m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[index]->EntityPtr))
					{
						m_Transform[index]->Position -= offset;
					}
				}
			}
		}
	}
}