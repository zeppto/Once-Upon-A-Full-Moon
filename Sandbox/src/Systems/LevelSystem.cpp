#include <mcspch.hpp>
#include "LevelSystem.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{

	void LevelSystem::Init()
	{
		m_World = Frosty::Application::Get().GetWorld().get();
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
	}

	void LevelSystem::OnEvent(Frosty::BaseEvent& e)
	{
		switch (e.GetEventType())
		{
		case Frosty::EventType::ExitCurrentLevel:
			OnExitLevelEvent(static_cast<Frosty::ExitLevelEvent&>(e));
			break;
		default:
			break;
		}
	}

	void LevelSystem::OnUpdate()
	{
		if (start)
		{
			m_Map.generateMap();
			m_CurrentRoome = m_Map.getRoom(m_PlayerPos);

			int rotation = 0;
			std::string texture = m_Map.getRoomTextur(m_PlayerPos, &rotation);
			Level::Room(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1], m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3], texture, rotation);
			start = false;
		}
	}

	void LevelSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);

			p_Total++;
		}
	}

	void LevelSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

		if (tempIndex > 0)
		{
			p_Total--;
			m_Transform[p_Total] = nullptr;

			//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

			if (p_Total > tempIndex)
			{
				std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
				p_EntityMap[entityToUpdate] = tempIndex;
			}

			p_EntityMap.erase(entity);
		}
	}

	void LevelSystem::OnExitLevelEvent(Frosty::ExitLevelEvent& e)
	{
		auto& ExitTranform = m_World->GetComponent<Frosty::ECS::CTransform>(e.GetExitEntity());
		//auto& ExitBBox = m_World->GetComponent<Frosty::ECS::CPhysics>(e.GetExitEntity());
		auto& ExitSide = m_World->GetComponent<Frosty::ECS::CLevelExit>(e.GetExitEntity());

		auto& PlayerTranform = m_World->GetComponent<Frosty::ECS::CTransform>(e.GetPlayerEntity());

		//temp level swap
		for (size_t i = 1; i < p_Total; i++)
		{
			if (!m_World->HasComponent<Frosty::ECS::CCamera>(m_Transform[i]->EntityPtr))
			{
				if (!m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr))
				{
					if (m_World->HasComponent<Frosty::ECS::CPhysics>(m_Transform[i]->EntityPtr))
					{
						if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
						{
							m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
						}
					}
					else if (m_World->HasComponent<Frosty::ECS::CMesh>(m_Transform[i]->EntityPtr))
					{
						// Enemy (In this case as only enemies has CHealth for now. Later have an Enemy component)
						if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
						{
							m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
						}
					}
				}
			}
		}
		if (ExitSide.ExitDirection == 0)
			m_PlayerPos += glm::ivec2(0, -1);
		if (ExitSide.ExitDirection == 1)
			m_PlayerPos += glm::ivec2(0, 1);
		if (ExitSide.ExitDirection == 2)
			m_PlayerPos += glm::ivec2(1, 0);
		if (ExitSide.ExitDirection == 3)
			m_PlayerPos += glm::ivec2(-1, 0);

		m_CurrentRoome = m_Map.getRoom(m_PlayerPos);

		int rotation = 0;
		std::string texture = m_Map.getRoomTextur(m_PlayerPos, &rotation);
		PlayerTranform.Position = Level::Room(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1], m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3], texture, rotation, ExitSide.ExitDirection);


	}
}


