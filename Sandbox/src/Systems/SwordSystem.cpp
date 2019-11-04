#include <mcspch.hpp>
#include "SwordSystem.hpp"

namespace MCS
{

	void SwordSystem::Init()
	{
		m_World = Frosty::Application::Get().GetWorld().get();
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CSword>(), true);
	}

	void SwordSystem::OnUpdate()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Sword[i]->Lifetime > 0.0f) m_Sword[i]->Lifetime -= Frosty::Time::DeltaTime();

			if (m_Sword[i]->Lifetime <= 0.0f)
			{
				if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Sword[i]->EntityPtr))
				{
					m_World->AddComponent<Frosty::ECS::CDestroy>(m_Sword[i]->EntityPtr);
				}
			}
		}
	}

	void SwordSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Sword[p_Total] = &world->GetComponent<Frosty::ECS::CSword>(entity);

			p_Total++;
		}
	}

	void SwordSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

		if (tempIndex > 0)
		{
			p_Total--;
			m_Sword[p_Total] = nullptr;

			//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

			if (p_Total > tempIndex)
			{
				std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Sword[p_EntityMap[entity]]->EntityPtr;
				p_EntityMap[entityToUpdate] = tempIndex;
			}

			p_EntityMap.erase(entity);
		}
	}
}