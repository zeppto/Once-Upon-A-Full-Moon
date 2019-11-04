#include <fypch.hpp>
#include <mcspch.hpp>
#include "FollowSystem.hpp"

void FollowSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CFollow>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CPhysics>(), true);
}

void FollowSystem::OnUpdate()
{
	for (size_t i = 1; i < p_Total; i++)
	{
		if (m_Follow[i]->Target)
		{
			float distance = glm::distance({ m_Transform[i]->Position.x, 0.0f, m_Transform[i]->Position.z }, m_Follow[i]->Target->Position);
			if (distance > m_Follow[i]->StopDistance)
			{
				m_Physics[i]->Velocity = glm::normalize(m_Follow[i]->Target->Position - glm::vec3(m_Transform[i]->Position.x, 0.0f, m_Transform[i]->Position.z)) * m_Physics[i]->Speed;
			}
			else
			{
				m_Physics[i]->Velocity = glm::vec3(0.0f);
			}
		}
	}
}

void FollowSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
	{
		p_EntityMap.emplace(entity, p_Total);

		auto& world = Frosty::Application::Get().GetWorld();
		m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
		m_Follow[p_Total] = &world->GetComponent<Frosty::ECS::CFollow>(entity);
		m_Physics[p_Total] = &world->GetComponent<Frosty::ECS::CPhysics>(entity);

		p_Total++;
	}
}

void FollowSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

	if (tempIndex > 0)
	{
		p_Total--;
		m_Transform[p_Total] = nullptr;
		m_Follow[p_Total] = nullptr;
		m_Physics[p_Total] = nullptr;

		if (p_Total > 1)
		{
			//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

			if (p_Total > tempIndex)
			{
				std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
				p_EntityMap[entityToUpdate] = tempIndex;
			}
		}

		p_EntityMap.erase(entity);
	}
}