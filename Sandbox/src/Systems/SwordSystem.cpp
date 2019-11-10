#include <mcspch.hpp>
#include "SwordSystem.hpp"

namespace MCS
{
	const std::string SwordSystem::NAME = "Sword";

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
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Sword[p_Total]->EntityPtr;
			m_Sword[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void SwordSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			Frosty::ECS::CSword* swordPtr = m_World->GetComponentAddress<Frosty::ECS::CSword>(entity);

			m_Sword[it->second] = swordPtr;
		}
	}

	std::string SwordSystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_Sword[i] << "\t" << m_Sword[i]->EntityPtr->Id << "\t\t" << m_Sword[i]->EntityPtr << "\t\t" << m_Sword[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}
}