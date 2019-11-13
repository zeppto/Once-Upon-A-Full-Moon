#include "mcspch.hpp"
#include "BossBehaviorSystem.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{
	const std::string BossBehaviorSystem::NAME = "Boss Behavior";

	void BossBehaviorSystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CBoss>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CFollow>(), true);

	}

	void BossBehaviorSystem::OnUpdate()
	{
		auto& world = Frosty::Application::Get().GetWorld();

		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Boss[i]->TargetList.size() > 0 && m_Boss[i]->Distracted == false)
			{
				//Set new target
				if (m_Boss[i]->Hunting == false)
				{
					if (m_Boss[i]->TargetList.size() == 1)
					{
						//Follow player
						m_Follow[i]->Target = &world->GetComponent<Frosty::ECS::CTransform>(m_Boss[i]->TargetList.at(m_Boss[i]->TargetList.size() - 1));
						m_Follow[i]->StopDistance = 1.0f;
					}
					else
					{
						//Follow the nearest bait
						int ShortestID = FindClosestBait(m_Transform[i]->Position, m_Boss[i]->TargetList);

						std::swap(m_Boss[i]->TargetList.at(0), m_Boss[i]->TargetList.at(ShortestID));

						m_Follow[i]->Target = &world->GetComponent<Frosty::ECS::CTransform>(m_Boss[i]->TargetList.at(0));
						m_Boss[i]->Hunting = true;
						m_Follow[i]->StopDistance = 0.1f;
					}
				}

				float distance = CalcDistance2D(m_Transform[i]->Position, world->GetComponent<Frosty::ECS::CTransform>(m_Boss[i]->TargetList.at(0)).Position);

				if (distance < 0.5 && m_Boss[i]->TargetList.size() > 1)
				{
					m_Boss[i]->Distracted = true;
					m_Boss[i]->DistractionTimer = Frosty::Time::CurrentTime();
				}
			}

			float diff = Frosty::Time::CurrentTime() - m_Boss[i]->DistractionTimer;

			//Eat bait
			if ((m_Boss[i]->Distracted == true) && (Frosty::Time::CurrentTime() - m_Boss[i]->DistractionTimer >= m_Boss[i]->DistractionTime) && m_Boss[i]->TargetList.size() > 1)
			{
				world->AddComponent<Frosty::ECS::CDestroy>(m_Boss[i]->TargetList.at(0));
				m_Boss[i]->TargetList.erase(m_Boss[i]->TargetList.begin());
				m_Boss[i]->Distracted = false;
				m_Boss[i]->DistractionTimer = Frosty::Time::CurrentTime();

				//Tried to set new target here but did not work, sets hunting = true instead and the next target is set in next frame
				m_Boss[i]->Hunting = false;
			}
		}
	}

	void BossBehaviorSystem::OnEvent(Frosty::BaseEvent& e)
	{
		switch (e.GetEventType())
		{
		case Frosty::EventType::BaitPlaced:
			OnBaitPlacedEvent(static_cast<Frosty::BaitPlacedEvent&>(e));
			break;
		default:
			break;
		}
	}

	void BossBehaviorSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Boss[p_Total] = &world->GetComponent<Frosty::ECS::CBoss>(entity);
			m_Follow[p_Total] = &world->GetComponent<Frosty::ECS::CFollow>(entity);

			p_Total++;
		}
	}

	void BossBehaviorSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_Boss[p_Total] = nullptr;
			m_Follow[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void BossBehaviorSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CBoss* bossPtr = world->GetComponentAddress<Frosty::ECS::CBoss>(entity);
			Frosty::ECS::CFollow* followPtr = world->GetComponentAddress<Frosty::ECS::CFollow>(entity);

			m_Transform[it->second] = transformPtr;
			m_Boss[it->second] = bossPtr;
			m_Follow[it->second] = followPtr;
		}
	}

	std::string BossBehaviorSystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_Boss[i] << "\t" << m_Boss[i]->EntityPtr->Id << "\t\t" << m_Boss[i]->EntityPtr << "\t\t" << m_Boss[i]->EntityPtr.use_count() << "\n";
			retInfo << "\t\t" << i << "\t" << m_Follow[i] << "\t" << m_Follow[i]->EntityPtr->Id << "\t\t" << m_Follow[i]->EntityPtr << "\t\t" << m_Follow[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}

	void BossBehaviorSystem::OnBaitPlacedEvent(Frosty::BaitPlacedEvent& e)
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			auto& entity = e.GetEntity();

			auto& world = Frosty::Application::Get().GetWorld();
			
			m_Boss[i]->TargetList.emplace(m_Boss[i]->TargetList.end() - 1, entity);

			//Follow the nearest bait
			int ShortestID = FindClosestBait(m_Transform[i]->Position, m_Boss[i]->TargetList);

			std::swap(m_Boss[i]->TargetList.at(0), m_Boss[i]->TargetList.at(ShortestID));

			m_Follow[i]->Target = &world->GetComponent<Frosty::ECS::CTransform>(m_Boss[i]->TargetList.at(0));
			m_Boss[i]->Hunting = true;
			m_Follow[i]->StopDistance = 0.1f;

		}
	}

	int BossBehaviorSystem::FindClosestBait(const glm::vec3& SelfPos, const std::vector<std::shared_ptr<Frosty::ECS::Entity>>& Baits)
	{
		auto& world = Frosty::Application::Get().GetWorld();

		float ShortestDistance = CalcDistance2D(world->GetComponent<Frosty::ECS::CTransform>(Baits.at(0)).Position, SelfPos);
		int ShortestID = 0;
		float distance = 0;

		for (uint8_t bait = 1; bait < Baits.size() - 1; bait++)
		{
			distance = CalcDistance2D(world->GetComponent<Frosty::ECS::CTransform>(Baits.at(bait)).Position, SelfPos);
			if (distance < ShortestDistance)
			{
				ShortestID = bait;
				ShortestDistance = distance;
			}
		}
		return ShortestID;
	}

	float BossBehaviorSystem::CalcDistance2D(const glm::vec3& pos1, const glm::vec3& pos2)
	{
		glm::vec2 p1;
		p1.x = pos1.x;
		p1.y = pos1.z;

		glm::vec2 p2;
		p2.x = pos2.x;
		p2.y = pos2.z;

		return glm::distance(p1, p2);
	}

}

