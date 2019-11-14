#include <mcspch.hpp>
#include "LootingSystem.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{
	const std::string LootingSystem::NAME = "Looting";

	void LootingSystem::Init()
	{
		m_World = Frosty::Application::Get().GetWorld().get();
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CLootable>(), true);
	}

	void LootingSystem::OnUpdate()
	{
		m_CoolDown += Frosty::Time::DeltaTime();
	}

	void LootingSystem::OnEvent(Frosty::BaseEvent & e)
	{
		switch (e.GetEventType())
		{
		case Frosty::EventType::PickUpAttempt:
			OnPickUpAttemptEvent(static_cast<Frosty::PickUpAttemptEvent&>(e));
			break;
		default:
			break;
		}
	}

	void LootingSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			m_Transform[p_Total] = &m_World->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Lootable[p_Total] = &m_World->GetComponent<Frosty::ECS::CLootable>(entity);
			m_Items[p_Total] = entity;
			p_Total++;
		}
	}

	void LootingSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Lootable[p_Total] = nullptr;
			m_Transform[p_Total] = nullptr;
			m_Items[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void LootingSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			Frosty::ECS::CTransform* transformPtr = m_World->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CLootable* lootPtr = m_World->GetComponentAddress<Frosty::ECS::CLootable>(entity);

			m_Transform[it->second] = transformPtr;
			m_Lootable[it->second] = lootPtr;
			m_Items[it->second] = entity;
		}
	}

	std::string LootingSystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_Transform[i] << "\t" << m_Transform[i]->EntityPtr->Id << "\t\t" << m_Transform[i]->EntityPtr << "\t\t" << m_Transform[i]->EntityPtr.use_count() << "\n";
			retInfo << "\t\t" << i << "\t" << m_Lootable[i] << "\t" << m_Lootable[i]->EntityPtr->Id << "\t\t" << m_Lootable[i]->EntityPtr << "\t\t" << m_Lootable[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}

	void LootingSystem::OnPickUpAttemptEvent(Frosty::PickUpAttemptEvent & e)
	{
		m_Player = e.GetEntity();

		for (size_t i = 1; i < p_Total; i++)
		{
			float distanceToItem = Distance2D(m_Transform[i]->Position, m_World->GetComponent<Frosty::ECS::CTransform>(m_Player).Position);

			if (distanceToItem < m_WorkingArea && m_CoolDown > 2.0f)
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::PickUpEvent>(Frosty::PickUpEvent(m_Items[i]));
				FY_INFO("{0}", distanceToItem);
			}
		}
	}

	float LootingSystem::Distance2D(glm::vec3 pos1, glm::vec3 pos2)
	{
		glm::vec2 position1;
		position1.x = pos1.x;
		position1.y = pos1.z;

		glm::vec2 position2;
		position2.x = pos2.x;
		position2.y = pos2.z;

		return glm::distance(position1, position2);
	}
}
