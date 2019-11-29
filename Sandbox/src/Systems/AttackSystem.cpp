#include <mcspch.hpp>
#include "AttackSystem.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{
	const std::string AttackSystem::NAME = "Attack";

	void AttackSystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CAttack>(), true);

		m_World = Frosty::Application::Get().GetWorld().get();
	}

	void AttackSystem::OnUpdate()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Attack[i]->Type == Frosty::ECS::CAttack::AttackType::Melee && Frosty::Time::CurrentTime() - m_Attack[i]->LifetimeTimer >= 0.1f && !m_World->HasComponent<Frosty::ECS::CPhysics>(m_Attack[i]->EntityPtr))
			{
				auto& attackTransform = m_World->GetComponent<Frosty::ECS::CTransform>(m_Attack[i]->EntityPtr);
				m_World->AddComponent<Frosty::ECS::CPhysics>(m_Attack[i]->EntityPtr, Frosty::AssetManager::GetBoundingBox("pCube1"), attackTransform.Scale);
			}
			else if (Frosty::Time::CurrentTime() - m_Attack[i]->LifetimeTimer >= m_Attack[i]->Lifetime && m_World->HasComponent<Frosty::ECS::CPhysics>(m_Attack[i]->EntityPtr))
			{
				if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Attack[i]->EntityPtr))
				{
					m_World->AddComponent<Frosty::ECS::CDestroy>(m_Attack[i]->EntityPtr);
				}
			}
		}
	}

	void AttackSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Attack[p_Total] = &world->GetComponent<Frosty::ECS::CAttack>(entity);

			p_Total++;
		}
	}

	void AttackSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Attack[p_Total]->EntityPtr;
			m_Attack[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void AttackSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CAttack* attackPtr = world->GetComponentAddress<Frosty::ECS::CAttack>(entity);

			m_Attack[it->second] = attackPtr;
		}
	}

	std::string AttackSystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_Attack[i] << "\t" << m_Attack[i]->EntityPtr->Id << "\t\t" << m_Attack[i]->EntityPtr << "\t\t" << m_Attack[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}
}