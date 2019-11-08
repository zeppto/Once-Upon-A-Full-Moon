#include <mcspch.hpp>
#include "WeaponSystem.hpp"

namespace MCS
{

	void WeaponSystem::Init()
	{
		m_World = Frosty::Application::Get().GetWorld().get();
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CAttack>(), true);
	}

	void WeaponSystem::OnUpdate()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (Frosty::Time::CurrentTime() - m_Attack[i]->LifetimeTimer >= m_Attack[i]->Lifetime)
			{
				if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Attack[i]->EntityPtr))
				{
					m_World->AddComponent<Frosty::ECS::CDestroy>(m_Attack[i]->EntityPtr);
					m_Attack[i]->LifetimeTimer = Frosty::Time::CurrentTime();
				}
			}
		}
	}

	void WeaponSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Attack[p_Total] = &world->GetComponent<Frosty::ECS::CAttack>(entity);

			p_Total++;
		}
	}

	void WeaponSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

		if (tempIndex > 0)
		{
			p_Total--;
			m_Attack[p_Total] = nullptr;

			//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

			if (p_Total > tempIndex)
			{
				std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Attack[p_EntityMap[entity]]->EntityPtr;
				p_EntityMap[entityToUpdate] = tempIndex;
			}

			p_EntityMap.erase(entity);
		}
	}
}