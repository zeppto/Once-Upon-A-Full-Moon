#include <mcspch.hpp>
#include "ArrowSystem.hpp"

namespace MCS
{

	void ArrowSystem::Init()
	{
		m_World = Frosty::Application::Get().GetWorld().get();
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CArrow>(), true);
	}

	void ArrowSystem::OnUpdate()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Arrow[i]->Lifetime > 0.0f) m_Arrow[i]->Lifetime -= Frosty::Time::DeltaTime();

			if (m_Arrow[i]->Lifetime <= 0.0f)
			{
				if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Arrow[i]->EntityPtr))
				{
					m_World->AddComponent<Frosty::ECS::CDestroy>(m_Arrow[i]->EntityPtr);
				}
			}
		}
	}

	void ArrowSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Arrow[p_Total] = &world->GetComponent<Frosty::ECS::CArrow>(entity);

			p_Total++;
		}
	}

	void ArrowSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

		if (tempIndex > 0)
		{
			p_Total--;
			m_Arrow[p_Total] = nullptr;

			//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

			if (p_Total > tempIndex)
			{
				std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Arrow[p_EntityMap[entity]]->EntityPtr;
				p_EntityMap[entityToUpdate] = tempIndex;
			}

			p_EntityMap.erase(entity);
		}
	}
}