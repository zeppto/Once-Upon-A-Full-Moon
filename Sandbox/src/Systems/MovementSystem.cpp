#include <fypch.hpp>
#include <mcspch.hpp>
#include "MovementSystem.hpp"

void MovementSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CMotion>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CCollision>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTag>(), true);
}

void MovementSystem::OnUpdate()
{
	for (size_t i = 1; i < p_Total; i++)
	{
		//for arrow
		if (m_Tag[i]->TagName == "arrow" || m_Tag[i]->TagName == "strongArrow")
		{
			m_Motion[i]->Velocity = m_Motion[i]->Direction * m_Motion[i]->Speed;
			m_Transform[i]->Position += m_Motion[i]->Velocity * Frosty::Time::DeltaTime();
		}
		else
		{


			if (m_Motion[i]->DashActive)
			{
				m_Motion[i]->DistanceDashed += glm::length(m_Motion[i]->Velocity * Frosty::Time::DeltaTime());
				if (m_Motion[i]->DistanceDashed >= m_Motion[i]->DASH_DISTANCE / 1000.0f)
				{
					m_Motion[i]->DashActive = false;
					m_Motion[i]->DistanceDashed = 0.0f;
				}
			}
			m_Transform[i]->Position += m_Motion[i]->Velocity * Frosty::Time::DeltaTime();

			for (size_t j = 1; j < p_Total; j++)
			{
				if (j != i)
				{
					//temp
					if (m_Tag[i]->TagName != m_Tag[j]->TagName || m_Tag[i]->TagName == "Enemy")
						//	if (m_Tag[i]->TagName != m_Tag[j]->TagName)
					{
						glm::vec3 finalCenterA = m_Transform[i]->Position + glm::vec3(m_Collision[i]->BoundingBox->pos[0], m_Collision[i]->BoundingBox->pos[1], m_Collision[i]->BoundingBox->pos[2]);
						glm::vec3 finalCenterB = m_Transform[j]->Position + glm::vec3(m_Collision[j]->BoundingBox->pos[0], m_Collision[j]->BoundingBox->pos[1], m_Collision[j]->BoundingBox->pos[2]);
						glm::vec3 finalLengthA = glm::vec3(m_Collision[i]->BoundingBox->halfSize[0], m_Collision[i]->BoundingBox->halfSize[1], m_Collision[i]->BoundingBox->halfSize[2]) * m_Transform[i]->Scale;
						glm::vec3 finalLengthB = glm::vec3(m_Collision[j]->BoundingBox->halfSize[0], m_Collision[j]->BoundingBox->halfSize[1], m_Collision[j]->BoundingBox->halfSize[2]) * m_Transform[j]->Scale;
						glm::vec3 offset = Frosty::CollisionDetection::AABBIntersecPushback(finalLengthA, finalCenterA, finalLengthB, finalCenterB);

						m_Transform[i]->Position -= offset;
					}
				}
			}
		}
	}
}

void MovementSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
	{
		p_EntityMap.emplace(entity, p_Total);

		auto& world = Frosty::Application::Get().GetWorld();
		m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
		m_Motion[p_Total] = &world->GetComponent<Frosty::ECS::CMotion>(entity);
		m_Collision[p_Total] = &world->GetComponent<Frosty::ECS::CCollision>(entity);
		m_Tag[p_Total] = &world->GetComponent<Frosty::ECS::CTag>(entity);

		p_Total++;
	}
}

void MovementSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

	if (tempIndex > 0)
	{
		p_Total--;
		m_Transform[p_Total] = nullptr;
		m_Motion[p_Total] = nullptr;
		m_Collision[p_Total] = nullptr;

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