#include <mcspch.hpp>
#include "CombatSystem.hpp"

void CombatSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CCollision>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTag>(), true);
	//
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CArrow>(), true);
}

void CombatSystem::OnInput()
{
	for (size_t i = 1; i < p_Total; i++)
	{
	}
}

void CombatSystem::OnUpdate()
{
	for (size_t i = 1; i < p_Total; i++)
	{
		for (size_t j = 1; j < p_Total; j++)
		{
			if (m_Tag[j]->TagName == "Enemy" && (m_Tag[i]->TagName == "arrow"))
			{

				glm::vec3 finalCenterA = m_Transform[i]->Position + glm::vec3(m_Collision[i]->BoundingBox->pos[0], m_Collision[i]->BoundingBox->pos[1], m_Collision[i]->BoundingBox->pos[2]);
				glm::vec3 finalCenterB = m_Transform[j]->Position + glm::vec3(m_Collision[j]->BoundingBox->pos[0], m_Collision[j]->BoundingBox->pos[1], m_Collision[j]->BoundingBox->pos[2]);
				glm::vec3 finalLengthA = glm::vec3(m_Collision[i]->BoundingBox->halfSize[0], m_Collision[i]->BoundingBox->halfSize[1], m_Collision[i]->BoundingBox->halfSize[2]) * m_Transform[i]->Scale;
				glm::vec3 finalLengthB = glm::vec3(m_Collision[j]->BoundingBox->halfSize[0], m_Collision[j]->BoundingBox->halfSize[1], m_Collision[j]->BoundingBox->halfSize[2]) * m_Transform[j]->Scale;
				if (Frosty::CollisionDetection::AABBIntersect(finalLengthA, finalCenterA, finalLengthB, finalCenterB))
				{
					if (m_Tag[i]->TagName == "arrow")
					{
						if (m_Arrow[i]->IsPiercing && m_Arrow[i]->alradyHitt == j)
						{
						}
						else
						{
							FY_TRACE("playerAttack ({0})", j);
							m_Health[j]->CurrentHealth -= m_Arrow[i]->Damage;
							m_Health[i]->CurrentHealth--;
							FY_TRACE("current health ({0})", m_Health[j]->CurrentHealth);
							if (m_Arrow[i]->IsPiercing)
								m_Arrow[i]->alradyHitt = j;
						}
					}

					//nockBack
					//enemy sak ta hand om sin egen push back, har sak bara skickas till enemyn att du ska push backa
					m_Transform[j]->Position.x -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).x * 1.0f;
					m_Transform[j]->Position.z -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).z * 1.0f;
				}
			}
		}
		//temp death timer
		if (m_Tag[i]->TagName == "arrow")
		{
			m_Arrow[i]->Lifetime--;
			if (m_Arrow[i]->Lifetime < 0)
				m_Health[i]->CurrentHealth--;
		}
	}
}

void CombatSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
	{
		p_EntityMap.emplace(entity, p_Total);

		auto& world = Frosty::Application::Get().GetWorld();
		m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
		m_Collision[p_Total] = &world->GetComponent<Frosty::ECS::CCollision>(entity);
		m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);
		m_Tag[p_Total] = &world->GetComponent<Frosty::ECS::CTag>(entity);
		//
		m_Arrow[p_Total] = &world->GetComponent<Frosty::ECS::CArrow>(entity);

		p_Total++;
	}
}

void CombatSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

	if (tempIndex > 0)
	{
		p_Total--;
		m_Transform[p_Total] = nullptr;
		m_Collision[p_Total] = nullptr;
		m_Health[p_Total] = nullptr;
		m_Tag[p_Total] = nullptr;
		//
		m_Arrow[p_Total] = nullptr;

		//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

		if (p_Total > tempIndex)
		{
			std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
			p_EntityMap[entityToUpdate] = tempIndex;
		}

		p_EntityMap.erase(entity);
	}
}