#include "mcspch.hpp"
#include "BossBehaviorSystem.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{
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
						m_Follow[i]->StopDistance = 1;
					}
					else
					{
						//Follow the nearest bait
						int ShortestID = FindClosestBait(m_Transform[i]->Position, m_Boss[i]->TargetList);

						std::swap(m_Boss[i]->TargetList.at(0), m_Boss[i]->TargetList.at(ShortestID));

						m_Follow[i]->Target = &world->GetComponent<Frosty::ECS::CTransform>(m_Boss[i]->TargetList.at(0));
						m_Boss[i]->Hunting = true;
						m_Follow[i]->StopDistance = 0.1;
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
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

		if (tempIndex > 0)
		{
			p_Total--;
			m_Boss[p_Total] = nullptr;
			m_Follow[p_Total] = nullptr;


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

	void BossBehaviorSystem::OnBaitPlacedEvent(Frosty::BaitPlacedEvent& e)
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			size_t index = p_EntityMap[e.GetEntity()];
			std::shared_ptr<Frosty::ECS::Entity> entity = e.GetEntity();


			auto& world = Frosty::Application::Get().GetWorld();


			if (m_Boss[i]->TargetList.size() > 0)
			{
				//Last pos is for player
				m_Boss[i]->TargetList.emplace(m_Boss[i]->TargetList.end() - 1, entity);
			}
			else
			{
				//Temp?
				m_Boss[i]->TargetList.emplace(m_Boss[i]->TargetList.end(), entity);

			}

			//Follow the nearest bait
			int ShortestID = FindClosestBait(m_Transform[i]->Position, m_Boss[i]->TargetList);

			std::swap(m_Boss[i]->TargetList.at(0), m_Boss[i]->TargetList.at(ShortestID));

			m_Follow[i]->Target = &world->GetComponent<Frosty::ECS::CTransform>(m_Boss[i]->TargetList.at(0));
			m_Boss[i]->Hunting = true;
			m_Follow[i]->StopDistance = 0.1;

		}
	}

	int BossBehaviorSystem::FindClosestBait(glm::vec3 SelfPos, std::vector<std::shared_ptr<Frosty::ECS::Entity>>& Baits)
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

	float BossBehaviorSystem::CalcDistance2D(glm::vec3 pos1, glm::vec3 pos2)
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

