#include <mcspch.hpp>
#include "ArrowSystem.hpp"

namespace MCS
{
	const std::string ArrowSystem::NAME = "Arrow";

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
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Arrow[p_Total]->EntityPtr;
			m_Arrow[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void ArrowSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CArrow* arrowPtr = world->GetComponentAddress<Frosty::ECS::CArrow>(entity);

			m_Arrow[it->second] = arrowPtr;
		}
	}

	std::string ArrowSystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_Arrow[i] << "\t" << m_Arrow[i]->EntityPtr->Id << "\t\t" << m_Arrow[i]->EntityPtr << "\t\t" << m_Arrow[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}
}