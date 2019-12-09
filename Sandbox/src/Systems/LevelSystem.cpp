#include <mcspch.hpp>
#include "LevelSystem.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/AbilityEvent.hpp"
#include "Frosty/Core/BoolMap/BoolMapGenerator.hpp"

namespace MCS
{
	const std::string LevelSystem::NAME = "Level";

	void LevelSystem::Init()
	{
		m_World = Frosty::Application::Get().GetWorld().get();
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
	}

	void LevelSystem::OnStart()
	{
		if (m_LoadMapBool)
		{
			m_LevelFileFormat.OpenFromFile(m_LoadFileName,
				false,
				m_OtherRoom,
				m_PlayerTransformLoadComponent,
				m_MapRotation,
				glm::vec3(0.0f, 0.0f, 0.0f),
				m_LoadExitDir);

			m_O_Room.RoomName = m_LoadFileName;
			m_O_Room.Rotation = m_MapRotation;


			m_PlayerTransformLoadComponent = nullptr;
			m_LoadMapBool = false;
		}

		if (m_LodeNamedRoom)
		{
			m_LodeNamedRoom = false;
		}
		if (m_ReStart)
		{
			m_ReStart = false;
			m_Start = true;
			m_World->DestroyGroup(false);
			m_OtherRoom = { -1, -1 };
			m_BossPos = { 9, 15 };
			m_BossRememberdPath.pathToGo.clear();
			m_BossRememberdPath.expectedPlayerPos = { -1, -1 };
			m_BossRememberdPath.lastTile = 1;
		}
	}

	void LevelSystem::OnEvent(Frosty::BaseEvent& e)
	{
		switch (e.GetEventType())
		{
		case Frosty::EventType::ExitCurrentLevel:
			OnExitLevelEvent(static_cast<Frosty::ExitLevelEvent&>(e));
			break;
		case Frosty::EventType::SaveLevel:
			OnSaveLevelEvent(static_cast<Frosty::SaveLevelEvent&>(e));
			break;
		case Frosty::EventType::CreateLevel:
			OnCreateLevelEvent(static_cast<Frosty::CreateLevelEvent&>(e));
			break;
		case Frosty::EventType::OpenLevel:
			OnOpenLevelEvent(static_cast<Frosty::OpenLevelEvent&>(e));
			break;
		case Frosty::EventType::CreatEntity:
			OnCreatEntityEvent(static_cast<Frosty::CreatEntityEvent&>(e));
			break;
		case Frosty::EventType::UpdatePlayerCoordsPos:
			OnPlayerUpdateCoordEvent(static_cast<Frosty::UpdatePlayerRoomCoordEvent&>(e));
			break;
		case Frosty::EventType::Reset:
			OnResetEvent(static_cast<Frosty::ResetEvent&>(e));
			break;
		case Frosty::EventType::BossSpawned:
			OnBossSpawnedEvent(static_cast<Frosty::BossSpawnedEvent&>(e));
			break;
		case Frosty::EventType::BaitPlaced:
			OnBaitPlacedEvent(static_cast<Frosty::BaitPlacedEvent&>(e));
			break;
		default:
			break;
		}
	}

	void LevelSystem::OnUpdate()
	{
		if (m_Start)
		{
			m_Map.generateMap();
			m_CurrentRoome = m_Map.getRoom(m_PlayerCoords);

			m_LevelFileFormat.clearVisitedRooms();

			//int rotation = 0;
			//std::string texture = m_Map.getRoomTextur(m_PlayerCoords, &rotation);
			//Level::Room(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1], m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3], texture, rotation);
			//this is curently the start room
			Frosty::ECS::CTransform* playerTransform = nullptr;
			for (size_t i = 1; i < p_Total; i++)
			{
				//addet to se if it fixes a crash
				if (m_Transform[i]->EntityPtr != nullptr)
				{
					if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr))
					{
						playerTransform = m_Transform[i];
					}
				}
			}
			int rotate;

			m_Map.getRoomTextur(m_PlayerCoords, &rotate);
			m_CurrentRoomBool = m_World->GetCurrentRoom();
			//	Level::MoveToNewRoom(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1], m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3]);
			m_LevelFileFormat.OpenFromFile("deadend_chests_IsStatick_t_p_e_r_h_a", !m_NextLevel, m_PlayerCoords, playerTransform, rotate);
			
			m_T_Room.RoomName = "deadend_chests_IsStatick_t_p_e_r_h_a";

			m_T_Room.Rotation = rotate;
			Frosty::EventBus::GetEventBus()->Publish<Frosty::UpdateCurrentRoomEvent>(Frosty::UpdateCurrentRoomEvent(m_T_Room.RoomName, m_T_Room.Rotation));
			Frosty::EventBus::GetEventBus()->Publish<Frosty::SwitchRoomEvent>(Frosty::SwitchRoomEvent());


			//m_RoomRotation = rotate;
			//m_ThisRoomName = "deadend_chests_IsStatick_t_p_e_r_h";
			m_Start = false;
			m_haveStartedMoving = false;

			//m_LevelFileFormat.LoadBoolMap("deadend_chests_IsStatick_t_p_e_r_h_a");
			m_StartTimer = Frosty::Time::CurrentTime();
		}

		float time = Frosty::Time::CurrentTime() - m_StartTimer - m_BossStartTimer;
		if (time > 0)
		{
			if (!m_haveStartedMoving)
			{
				FY_INFO("The boss have started moving {0}", "!");
				Room chekRoom = m_Map.getRoom(m_BossPos);
				if (!chekRoom.Ocupide)
				{
					m_BossPos = m_Map.getLastCreatedLevelPos();
				}
				FY_INFO("The boss starts on ({0}, {1})", m_BossPos.x, m_BossPos.y);
				m_haveStartedMoving = true;
			}

			if (time > m_BossTimer)
			{
				float estematedBossRoomTime;
				if (m_LevelFileFormat.NumberOfRoomsVisited() != 0)
				{
					estematedBossRoomTime = ((Frosty::Time::CurrentTime() - m_StartTimer + 50.0f) / m_LevelFileFormat.NumberOfRoomsVisited() / 2);
					if(estematedBossRoomTime > m_BossRoomTimer)
						estematedBossRoomTime = m_BossRoomTimer;
				}
				else
				{
					estematedBossRoomTime = m_BossRoomTimer;
				}

				m_BossTimer = time + estematedBossRoomTime;
				FY_INFO("It takes {0} sec until boss moves", estematedBossRoomTime);
				if (m_PlayerCoords != m_BossPos)
				{
					if (m_RoomswhithBait.size() > 0)
					{
						//follow bait
						FY_INFO("The boss is following bait");
						Room bossCurrentRoom = m_Map.getRoom(m_BossPos);
						glm::ivec2 expectedBossPos = glm::ivec2(-1, -1);
						if (m_BossRememberdPath.pathToGo.size() > m_BossRememberdPath.lastTile)
							expectedBossPos = m_BossRememberdPath.pathToGo[m_BossRememberdPath.lastTile - 1];
						//the boss follows bait
						for (int i = 0; i < m_RoomswhithBait.size(); i++)
						{
							if (m_BossRememberdPath.expectedPlayerPos != m_RoomswhithBait[i] || m_BossPos != expectedBossPos)
							{
								bossRememberdPath baitTarget;
								baitTarget.pathToGo = m_Map.getPathToTargert(m_BossPos, m_RoomswhithBait[i]);
								baitTarget.lastTile = 1;
								baitTarget.expectedPlayerPos = m_RoomswhithBait[i];
								if (baitTarget.pathToGo.size() - baitTarget.lastTile < m_BossRememberdPath.pathToGo.size() - m_BossRememberdPath.lastTile || m_BossRememberdPath.pathToGo.size() - m_BossRememberdPath.lastTile == 0)
								{
									FY_INFO("The boss found a closer bait");
									m_BossRememberdPath.pathToGo = baitTarget.pathToGo;
									m_BossRememberdPath.lastTile = baitTarget.lastTile;
									m_BossRememberdPath.expectedPlayerPos = baitTarget.expectedPlayerPos;
								}
							}
						}
						if (m_BossRememberdPath.pathToGo.size() > m_BossRememberdPath.lastTile)
						{
							m_BossPos = m_BossRememberdPath.pathToGo[m_BossRememberdPath.lastTile];
							m_BossRememberdPath.lastTile++;
							FY_INFO("The boss is moving to ({0}, {1})", m_BossPos.x, m_BossPos.y);
							FY_INFO("");

						}
						else
						{
							FY_INFO("Somthing is wrong and the boss did don't move");
							FY_INFO("");
						}
						if (m_BossRememberdPath.expectedPlayerPos == m_BossPos)
						{
							float timeToEate = (5.0f * m_LevelFileFormat.RemoveAllBaitInRoom(m_BossPos));
							m_BossTimer += timeToEate;
							for (int i = 0; i < m_RoomswhithBait.size(); i++)
							{
								if (m_RoomswhithBait[i] == m_BossPos)
									m_RoomswhithBait.erase(m_RoomswhithBait.begin() + i);
							}
							FY_INFO("The boss found the bait! and is eating for {0} sec", timeToEate);
							FY_INFO("");
						}

						if (m_BossPos == m_PlayerCoords)
						{
							Frosty::EventBus::GetEventBus()->Publish<Frosty::SpawnBossEvent>(Frosty::SpawnBossEvent());
							FY_INFO("The boss found the player!");
							FY_INFO("");
						}
					}
					else
					{
						//ther is no bait so move to player
						int moveToPlayerChanse = rand() % 100;
						FY_INFO("The boss hase {0} % chans to move to player", time / ((m_BossFollowTimer * 60.0f) / 100.0f));
						if (moveToPlayerChanse < time / ((m_BossFollowTimer * 60.0f) / 100.0f))
						{
							FY_INFO("The boss knowes wher you are and moves");
							Room bossCurrentRoom = m_Map.getRoom(m_BossPos);
							glm::ivec2 expectedBossPos = glm::ivec2(-1, -1);
							if (m_BossRememberdPath.pathToGo.size() > m_BossRememberdPath.lastTile)
								expectedBossPos = m_BossRememberdPath.pathToGo[m_BossRememberdPath.lastTile - 1];
							//the boss follows player
							if (m_BossRememberdPath.expectedPlayerPos != m_PlayerCoords || m_BossPos != expectedBossPos)
							{
								m_BossRememberdPath.pathToGo = m_Map.getPathToTargert(m_BossPos, m_PlayerCoords);
								m_BossRememberdPath.lastTile = 1;
								m_BossRememberdPath.expectedPlayerPos = m_PlayerCoords;

							}
							if (m_BossRememberdPath.pathToGo.size() > m_BossRememberdPath.lastTile)
							{
								m_BossPos = m_BossRememberdPath.pathToGo[m_BossRememberdPath.lastTile];
								m_BossRememberdPath.lastTile++;
								FY_INFO("The boss is moving to ({0}, {1})", m_BossPos.x, m_BossPos.y);
								FY_INFO("");

							}
							else
							{
								FY_INFO("Somthing is wrong and the boss did don't move");
								FY_INFO("");
							}
						}
						else
						{
							FY_INFO("The boss lost track of you and is serching");
							randomBossMovment();
							FY_INFO("");
						}

						if (m_BossPos == m_PlayerCoords)
						{
							Frosty::EventBus::GetEventBus()->Publish<Frosty::SpawnBossEvent>(Frosty::SpawnBossEvent());
							FY_INFO("The boss found the player!");
							FY_INFO("");
						}
					}
				}
			}

			//howel
			if(time > m_BossHawol && m_PlayerCoords != m_BossPos)
			{
				FY_INFO("The boss haoweld");
				FY_INFO("");
				int nextHawol = (rand() % 10) + 20;
				m_BossHawol = time + nextHawol;
				glm::vec2 direction = m_BossPos - m_PlayerCoords;

				Frosty::EventBus::GetEventBus()->Publish<Frosty::BossFearEffectEvent>(Frosty::BossFearEffectEvent(direction));
			}
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
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void LevelSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);

			m_Transform[it->second] = transformPtr;
		}
	}

	std::string LevelSystem::GetInfo() const
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
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}

	void LevelSystem::OnExitLevelEvent(Frosty::ExitLevelEvent& e)
	{
		if (m_BossSpawned) return;

		auto& ExitTranform = m_World->GetComponent<Frosty::ECS::CTransform>(e.GetExitEntity());
		//auto& ExitBBox = m_World->GetComponent<Frosty::ECS::CPhysics>(e.GetExitEntity());
		auto& ExitSide = m_World->GetComponent<Frosty::ECS::CLevelExit>(e.GetExitEntity());

		auto& PlayerTranform = m_World->GetComponent<Frosty::ECS::CTransform>(e.GetPlayerEntity());
		//Frosty::ECS::CTransform playerTransform = static_cast<Frosty::ECS::CTransform>(PlayerTranform);

		////temp level swap
		//for (size_t i = 1; i < p_Total; i++)
		//{
		//	if (m_Transform[i]->EntityPtr != nullptr)
		//	{
		//		if (!m_World->HasComponent<Frosty::ECS::CCamera>(m_Transform[i]->EntityPtr))
		//		{
		//			if (!m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr))
		//			{
		//				if (!m_World->HasComponent<Frosty::ECS::CWeapon>(m_Transform[i]->EntityPtr))
		//				{
		//					if (m_World->HasComponent<Frosty::ECS::CParticleSystem>(m_Transform[i]->EntityPtr))
		//					{
		//						if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
		//						{
		//							m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
		//						}
		//					}
		//					else if (m_World->HasComponent<Frosty::ECS::CPhysics>(m_Transform[i]->EntityPtr))
		//					{
		//						if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
		//						{
		//							m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
		//						}
		//					}
		//					else if (m_World->HasComponent<Frosty::ECS::CMesh>(m_Transform[i]->EntityPtr))
		//					{
		//						if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
		//						{
		//							m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
		//						}
		//					}
		//					else if (m_World->HasComponent<Frosty::ECS::CLight>(m_Transform[i]->EntityPtr))
		//					{
		//						auto& light = m_World->GetComponent<Frosty::ECS::CLight>(m_Transform[i]->EntityPtr);
		//						if (light.Type == Frosty::ECS::CLight::LightType::Point)
		//							if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
		//							{
		//								m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
		//							}
		//					}
		//				}
		//			}
		//			else
		//			{
		//				playerTransform = m_Transform[i];
		//			}
		//		}
		//	}
		//}



		glm::ivec2 tempCoord = m_PlayerCoords;

		if (ExitSide.ExitDirection == 0)
			tempCoord += glm::ivec2(0, -1);
		if (ExitSide.ExitDirection == 1)
			tempCoord += glm::ivec2(0, 1);
		if (ExitSide.ExitDirection == 2)
			tempCoord += glm::ivec2(-1, 0);
		if (ExitSide.ExitDirection == 3)

		tempCoord += glm::ivec2(1, 0);
		
/*
		m_PlayerCoords += glm::ivec2(1, 0);
		FY_INFO("The player moved to ({0}, {1})", m_PlayerCoords.x, m_PlayerCoords.y);
		FY_INFO("");
*/

		if (tempCoord != m_OtherRoom)
		{
			m_OtherRoom = tempCoord;
			//m_CurrentRoome = m_Map.getRoom(tempCoord);
			//m_EntrensSide = ExitSide.ExitDirection;
			//m_NextLevel = true;
			//m_TempTimer = 0;

			//int rotation = 0;

			m_LoadMapBool = true;
			m_LoadExitDir = ExitSide.ExitDirection;
			m_World->DestroyGroup(false);
			m_PlayerTransformLoadComponent = &PlayerTranform;
			m_LoadFileName = m_Map.getRoomTextur(m_OtherRoom, &m_MapRotation);
			//PlayerTranform.Position = Level::Room(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1], 
			//	m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3], texture, rotation, ExitSide.ExitDirection);
			//PlayerTranform.Position = Level::MoveToNewRoom(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1],
			//	m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3], ExitSide.ExitDirection);
			//m_LevelFileFormat.OpenFromFile(fileName, m_PlayerCoords, playerTransform, rotation);

		}
/*
		m_PlayerTransform = playerTransform;

		//int rotation = 0;
		//std::string fileName = m_Map.getRoomTextur(m_PlayerCoords, &rotation);
		//PlayerTranform.Position = Level::Room(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1], 
		//	m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3], texture, rotation, ExitSide.ExitDirection);
		PlayerTranform.Position = Level::MoveToNewRoom(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1],
			m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3], ExitSide.ExitDirection);
		//m_LevelFileFormat.OpenFromFile(fileName, m_PlayerCoords, playerTransform, rotation);
		m_CreatNewRoom = true;
*/

	}

	void LevelSystem::OnSaveLevelEvent(Frosty::SaveLevelEvent& e)
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (!m_World->HasComponent<Frosty::ECS::CCamera>(m_Transform[i]->EntityPtr))
			{
				if (!m_World->HasComponent<Frosty::ECS::CGUI>(m_Transform[i]->EntityPtr))
				{
					if (!m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr))
					{
						if (!m_World->HasComponent<Frosty::ECS::CWeapon>(m_Transform[i]->EntityPtr))
						{

							if (m_World->HasComponent<Frosty::ECS::CLight>(m_Transform[i]->EntityPtr))
							{
								auto& light = m_World->GetComponent<Frosty::ECS::CLight>(m_Transform[i]->EntityPtr);
								if (light.Type == Frosty::ECS::CLight::LightType::Point)
									m_LevelFileFormat.AddEntity(m_Transform[i]->EntityPtr);
							}
							else
							{
								m_LevelFileFormat.AddEntity(m_Transform[i]->EntityPtr);
							}
						}
					}
				}
			}
		}
		m_LevelFileFormat.SaveToFile(m_RoomType);
	}

	void LevelSystem::OnCreateLevelEvent(Frosty::CreateLevelEvent& e)
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (!m_World->HasComponent<Frosty::ECS::CCamera>(m_Transform[i]->EntityPtr))
			{
				if (!m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr))
				{
					if (!m_World->HasComponent<Frosty::ECS::CWeapon>(m_Transform[i]->EntityPtr))
					{
						if (m_World->HasComponent<Frosty::ECS::CParticleSystem>(m_Transform[i]->EntityPtr))
						{
							if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
							{
								m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
							}
						}
						else if (m_World->HasComponent<Frosty::ECS::CPhysics>(m_Transform[i]->EntityPtr))
						{
							if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
							{
								m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
							}
						}
						else if (m_World->HasComponent<Frosty::ECS::CMesh>(m_Transform[i]->EntityPtr))
						{
							if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
							{
								m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
							}
						}
						else if (m_World->HasComponent<Frosty::ECS::CLight>(m_Transform[i]->EntityPtr))
						{
							auto& light = m_World->GetComponent<Frosty::ECS::CLight>(m_Transform[i]->EntityPtr);
							if (light.Type == Frosty::ECS::CLight::LightType::Point)
								if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
								{
									m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
								}
						}
					}
					//else remove weapon if it isent player weapon
				}
			}
		}
		Level::Room(e.GetDirections(0), e.GetDirections(1), e.GetDirections(3), e.GetDirections(2));
		if (e.GetDirections(0) && !e.GetDirections(1) && !e.GetDirections(3) && !e.GetDirections(2))
			m_RoomType = "deadend";
		else if (e.GetDirections(0) && !e.GetDirections(1) && !e.GetDirections(3) && e.GetDirections(2))
			m_RoomType = "turningRoad";
		else if (!e.GetDirections(0) && !e.GetDirections(1) && e.GetDirections(3) && e.GetDirections(2))
			m_RoomType = "straightRoad";
		else if (e.GetDirections(0) && !e.GetDirections(1) && e.GetDirections(3) && e.GetDirections(2))
			m_RoomType = "threeWayRoad";
		else if (e.GetDirections(0) && e.GetDirections(1) && e.GetDirections(3) && e.GetDirections(2))
			m_RoomType = "crossroad";
		else
			m_RoomType = "unknown";
	}

	void LevelSystem::OnOpenLevelEvent(Frosty::OpenLevelEvent& e)
	{
		Frosty::ECS::CTransform* playerTransform = nullptr;
		for (size_t i = 1; i < p_Total; i++)
		{
			if (!m_World->HasComponent<Frosty::ECS::CCamera>(m_Transform[i]->EntityPtr))
			{
				if (!m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr))
				{
					if (!m_World->HasComponent<Frosty::ECS::CWeapon>(m_Transform[i]->EntityPtr))
					{
						if (m_World->HasComponent<Frosty::ECS::CParticleSystem>(m_Transform[i]->EntityPtr))
						{
							if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
							{
								m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
							}
						}
						else if (m_World->HasComponent<Frosty::ECS::CPhysics>(m_Transform[i]->EntityPtr))
						{
							if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
							{
								m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
							}
						}
						else if (m_World->HasComponent<Frosty::ECS::CMesh>(m_Transform[i]->EntityPtr))
						{
							if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
							{
								m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
							}
						}
						else if (m_World->HasComponent<Frosty::ECS::CLight>(m_Transform[i]->EntityPtr))
						{
							auto& light = m_World->GetComponent<Frosty::ECS::CLight>(m_Transform[i]->EntityPtr);
							if (light.Type == Frosty::ECS::CLight::LightType::Point)
							{
								if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
								{
									m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
								}
							}
						}
						else
						{
							if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
							{
								m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
							}
						}
					}
					//else remove weapon if it isent player weapon
				}
				else
				{
					playerTransform = m_Transform[i];
				}
			}
		}
		m_LodeNamedRoom = true;
		m_RoomType = e.GetFilename();

		m_LevelFileFormat.OpenFromFile(m_RoomType, 1, m_PlayerCoords, playerTransform);
		//For OpenFromFile second parameter.
		FY_ASSERT(0, "In use?");

	}

	void LevelSystem::OnCreatEntityEvent(Frosty::CreatEntityEvent& e)
	{
		//Enemy
		if (e.GetEntityType() == 0)
		{
			auto& enemy = m_World->CreateEntity({ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
			auto& enemyTransform = m_World->GetComponent<Frosty::ECS::CTransform>(enemy);
			m_World->AddComponent<Frosty::ECS::CMesh>(enemy, Frosty::AssetManager::GetMesh("pCube1"));
			m_World->AddComponent<Frosty::ECS::CMaterial>(enemy, Frosty::AssetManager::GetShader("FlatColor"));
			m_World->AddComponent<Frosty::ECS::CPhysics>(enemy, Frosty::AssetManager::GetBoundingBox("pCube1"), enemyTransform.Scale, 6.0f);
			m_World->AddComponent<Frosty::ECS::CEnemy>(enemy);
			//m_World->AddComponent<Frosty::ECS::CFollow>(enemy);
			m_World->AddComponent<Frosty::ECS::CHealth>(enemy);
		}
		//Stone
		if (e.GetEntityType() == 1)
		{
			auto& stone = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 90.0f }, { 3.0f, 3.0f, 3.0f });
			auto& stoneTransform = m_World->GetComponent<Frosty::ECS::CTransform>(stone);
			m_World->AddComponent<Frosty::ECS::CMesh>(stone, Frosty::AssetManager::GetMesh("stone1"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(stone, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("stoneTexture2");
			m_World->AddComponent<Frosty::ECS::CPhysics>(stone, Frosty::AssetManager::GetBoundingBox("stone1"), stoneTransform.Scale, 0.0f);
		}
		//Tree 
		if (e.GetEntityType() == 2)
		{
			auto& tree = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			auto& treeTransform = m_World->GetComponent<Frosty::ECS::CTransform>(tree);
			m_World->AddComponent<Frosty::ECS::CMesh>(tree, Frosty::AssetManager::GetMesh("tree1"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(tree, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");
			m_World->AddComponent<Frosty::ECS::CPhysics>(tree, Frosty::AssetManager::GetBoundingBox("tree1"), treeTransform.Scale, 0.0f);
		}
		//Mushroom 
		if (e.GetEntityType() == 3)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("mashrom1"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("mashRoomCirkel");
		}
		//Mushroom cirkel
		if (e.GetEntityType() == 4)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.1f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("hexCircle"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("mashRoomCirkel");
			auto& particel = m_World->AddComponent<Frosty::ECS::CParticleSystem>(mushroom, "ParticlesHorizontal", "particleRing", 3, glm::vec3(0.1f, 0.5f, 0.58f), 0.0f);
			particel.SystemEndColor = glm::vec3(0.43f, 0.145f, 0.145f);
			particel.StartParticleSize = 3.0f;
			particel.EndParticleSize = 8.0f;
			particel.EmitRate = 1.0;
			particel.EmitCount = 1;
			particel.FadeInTreshold = 1.915f;
			particel.FadeTreshold = 0.902f;
			particel.ParticleSystemStartPos = glm::vec3(0, 0.03f, 0);
			m_World->AddComponent<Frosty::ECS::CWitchCircle>(mushroom);
			m_World->AddComponent<Frosty::ECS::CLight>(mushroom, Frosty::ECS::CLight::LightType::Point, 5.0f, glm::vec3(0.1f, 1.f, 0.5f), 5.0f);
			m_World->AddComponent<Frosty::ECS::CHealth>(mushroom, 200, 0);
			auto& barComp = m_World->AddComponent<Frosty::ECS::CHealthBar>(mushroom, glm::vec3(0.0f, 20.0f, -5.0f), Frosty::AssetManager::GetMesh("UIPlane"), Frosty::AssetManager::GetShader("HealthBar"), Frosty::AssetManager::GetTexture2D("yellow"));
		}
		//mushroomsAndStonesBig
		if (e.GetEntityType() == 5)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			auto& mushroomTransform = m_World->GetComponent<Frosty::ECS::CTransform>(mushroom);
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("mushroomsAndStonesBig"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("stoneAndMushRooms");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("mushroomsAndStonesBig"), mushroomTransform.Scale, 0.0f);
		}
		//mushroomsAndStonesSmall
		if (e.GetEntityType() == 6)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			auto& mushroomTransform = m_World->GetComponent<Frosty::ECS::CTransform>(mushroom);
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("mushroomsAndStonesSmall"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("stoneAndMushRooms");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("mushroomsAndStonesSmall"), mushroomTransform.Scale, 0.0f);
		}
		//mushrooms
		if (e.GetEntityType() == 7)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("mushrooms"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("mashRoomCirkel");
		}
		//mushroomLong
		if (e.GetEntityType() == 8)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("mushroomLongTree"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("mashRoomCirkel");
		}
		//mushroomsFlat
		if (e.GetEntityType() == 9)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("mushroomsFlat"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("mashRoomCirkel");
		}
		//2stones
		if (e.GetEntityType() == 10)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			auto& mushroomTransform = m_World->GetComponent<Frosty::ECS::CTransform>(mushroom);
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("stones2"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("stoneTexture2");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("stones2"), mushroomTransform.Scale, 0.0f);
		}
		//3stones
		if (e.GetEntityType() == 11)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			auto& mushroomTransform = m_World->GetComponent<Frosty::ECS::CTransform>(mushroom);
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("stones3"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("stoneTexture2");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("stones3"), mushroomTransform.Scale, 0.0f);
		}
		//stones4
		if (e.GetEntityType() == 12)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			auto& mushroomTransform = m_World->GetComponent<Frosty::ECS::CTransform>(mushroom);
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("stones4"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("stoneTexture2");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("stones4"), mushroomTransform.Scale, 0.0f);
		}
		//treeBunch3
		if (e.GetEntityType() == 13)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			auto& mushroomTransform = m_World->GetComponent<Frosty::ECS::CTransform>(mushroom);
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("treeBunch3"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("treeBunch3"), mushroomTransform.Scale, 0.0f);
		}
		//treeBunch4
		if (e.GetEntityType() == 14)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			auto& mushroomTransform = m_World->GetComponent<Frosty::ECS::CTransform>(mushroom);
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("treeBunch4"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("treeBunch4"), mushroomTransform.Scale, 0.0f);
		}
		//treeBunch7
		if (e.GetEntityType() == 15)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			auto& mushroomTransform = m_World->GetComponent<Frosty::ECS::CTransform>(mushroom);
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("treeBunch7"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("treeBunch7"), mushroomTransform.Scale, 0.0f);
		}
		//treeBunchWall
		if (e.GetEntityType() == 16)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			auto& mushroomTransform = m_World->GetComponent<Frosty::ECS::CTransform>(mushroom);
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("treeBunchWall"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("treeBunchWall"), mushroomTransform.Scale, 0.0f);
		}
		//chest
		if (e.GetEntityType() == 17)
		{
			auto& chest = m_World->CreateEntity({ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			auto& chestTransform = m_World->GetComponent<Frosty::ECS::CTransform>(chest);
			m_World->AddComponent<Frosty::ECS::CMesh>(chest, Frosty::AssetManager::GetMesh("chest"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(chest, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("chest");
			m_World->AddComponent<Frosty::ECS::CPhysics>(chest, Frosty::AssetManager::GetBoundingBox("chest"), chestTransform.Scale, 6.0f);
			m_World->AddComponent<Frosty::ECS::CHealth>(chest, 2);
			m_World->AddComponent<Frosty::ECS::CDropItem>(chest);
			//world->AddComponent<Frosty::ECS::CLight>(torch, Frosty::ECS::CLight::LightType::Point, 1.f, glm::vec3(0.99f, 0.9f, 0.8f), &playerTransform, glm::vec3(0.f, 5.f, 0.f));
			m_World->AddComponent<Frosty::ECS::CLight>(chest, Frosty::ECS::CLight::LightType::Point, 1.0f, glm::vec3(1.0f, 0.99f, 0.95f), 5.0f, glm::vec3(0.f, 1.0f, 0.f));
			////CParticleSystem(const std::string shaderName, const std::string texName, unsigned int maxParticles, const glm::vec3& color, float particleSpeed)
			auto& particel = m_World->AddComponent<Frosty::ECS::CParticleSystem>(chest, "ParticlesHorizontal", "particle", 3, glm::vec3(1.0f, 0.96f, 0.0f), 0.0f);
			particel.EndParticleSize = 10.0f;
			particel.FadeInTreshold = 0.94f;
			particel.FadeTreshold = 1.32f;
			particel.EmitRate = 1.0f;
		}
		//spars fire flises
		if (e.GetEntityType() == 18)
		{
			auto& fireFlise = m_World->CreateEntity({ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 10.0f, 1.0f, 10.0f });
			auto& particel = m_World->AddComponent<Frosty::ECS::CParticleSystem>(fireFlise, "Particles", "particle", 50, glm::vec3(0.835f, 0.83f, 0.34f), 0.02f);
			particel.SystemEndColor = glm::vec3(0.78f, 0.5f, 0.09f);
			particel.RandomDirection = true;
			particel.randSpread = 3.0f;
			particel.randMainDir = glm::vec3(0, 0, 1);
			particel.RandomStartPos = true;
			particel.StartParticleSize = 0.8f;
			particel.EmitRate = 0.3f;
			particel.EmitCount = 5;
			particel.FadeInTreshold = 2.622f;
			particel.FadeTreshold = 2.675f;
		}
		//fire flises
		if (e.GetEntityType() == 19)
		{
			auto& fireFlise = m_World->CreateEntity({ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 10.0f, 1.0f, 10.0f });
			m_World->AddComponent<Frosty::ECS::CLight>(fireFlise, Frosty::ECS::CLight::LightType::Point, 0.37f, glm::vec3(1.0f, 0.98f, 0.91f), 23.0f, glm::vec3(0.f, 1.0f, 0.f));
			auto& particel = m_World->AddComponent<Frosty::ECS::CParticleSystem>(fireFlise, "Particles", "particle", 80, glm::vec3(0.835f, 0.83f, 0.34f), 0.05f);
			particel.SystemEndColor = glm::vec3(0.78f, 0.5f, 0.09f);
			particel.RandomDirection = true;
			particel.randSpread = 3.0f;
			particel.randMainDir = glm::vec3(0, 0, 1);
			particel.RandomStartPos = true;
			particel.StartParticleSize = 0.8f;
			particel.EmitRate = 0.1f;
			particel.EmitCount = 12;
			particel.FadeInTreshold = 2.622f;
			particel.FadeTreshold = 2.675f;
		}
		//swarm fire flises
		if (e.GetEntityType() == 20)
		{
			auto& fireFlise = m_World->CreateEntity({ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 3.0f, 1.0f, 3.0f });
			m_World->AddComponent<Frosty::ECS::CLight>(fireFlise, Frosty::ECS::CLight::LightType::Point, 0.62f, glm::vec3(1.0f, 0.98f, 0.91f), 23.0f, glm::vec3(0.f, 1.0f, 0.f));
			auto& particel = m_World->AddComponent<Frosty::ECS::CParticleSystem>(fireFlise, "Particles", "particle", 80, glm::vec3(0.835f, 0.83f, 0.34f), 0.05f);
			particel.SystemEndColor = glm::vec3(0.78f, 0.5f, 0.09f);
			particel.RandomDirection = true;
			particel.randSpread = 3.0f;
			particel.randMainDir = glm::vec3(0, 0, 1);
			particel.RandomStartPos = true;
			particel.StartParticleSize = 0.8f;
			particel.EmitRate = 0.1f;
			particel.EmitCount = 12;
			particel.FadeInTreshold = 2.622f;
			particel.FadeTreshold = 2.675f;
		}
		//fire flises blue
		if (e.GetEntityType() == 21)
		{
			auto& fireFlise = m_World->CreateEntity({ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 3.0f, 1.0f, 3.0f });
			m_World->AddComponent<Frosty::ECS::CLight>(fireFlise, Frosty::ECS::CLight::LightType::Point, 0.72f, glm::vec3(0.1f, 0.7f, 0.96f), 23.0f, glm::vec3(0.f, 1.0f, 0.f));
			auto& particel = m_World->AddComponent<Frosty::ECS::CParticleSystem>(fireFlise, "Particles", "particle", 9, glm::vec3(0.34f, 0.835f, 0.82f), 0.3f);
			particel.SystemEndColor = glm::vec3(0.07f, 0.21f, 0.86f);
			particel.RandomDirection = true;
			particel.randSpread = 5.0f;
			particel.randMainDir = glm::vec3(0, 1, 0);
			particel.RandomStartPos = true;
			particel.StartParticleSize = 1.0f;
			particel.EndParticleSize = 0.3f;
			particel.EmitRate = 1.0f;
			particel.EmitCount = 3;
			particel.FadeInTreshold = 2.589f;
			particel.FadeTreshold = 0.305f;
		}
		// enemy cultIst
		if (e.GetEntityType() == 22)
		{
			auto& enemyWeaponA = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			//world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("pCube1"));
			//world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("FlatColor"));
			auto& enemyWeaponCompA = m_World->AddComponent<Frosty::ECS::CWeapon>(enemyWeaponA, Frosty::ECS::CWeapon::WeaponType::Bow, 1, 1.0f);
			enemyWeaponCompA.LVL1AttackCooldown = 3.0f;
			//enemyWeaponCompA.MaxAttackRange = 5.0f;
			//enemyWeaponCompA.MinAttackRange = 0.0f;
			enemyWeaponCompA.MaxAttackRange = 22.0f;
			enemyWeaponCompA.MinAttackRange = 18.0f;
			//enemyWeaponCompA.AttackHitboxScale = glm::vec3(10.0f, 6.0f, 4.0f);				// Sword
			//enemyWeaponCompA.AttackHitboxScale = glm::vec3(4.0f, 6.0f, 4.0f);				// Bite
			enemyWeaponCompA.AttackHitboxScale = glm::vec3(0.3f);							// Arrow


			// ENEMY A
			auto& enemyA = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
			auto& enemyATransform = m_World->GetComponent<Frosty::ECS::CTransform>(enemyA);
			//world->AddComponent<Frosty::ECS::CAnimController>(enemyA).currAnim = Frosty::AssetManager::GetAnimation("Wolf_Running");
			m_World->AddComponent<Frosty::ECS::CMesh>(enemyA, Frosty::AssetManager::GetMesh("Cultist"));
			auto& enemyMatA = m_World->AddComponent<Frosty::ECS::CMaterial>(enemyA, Frosty::AssetManager::GetShader("Texture2D"));
			enemyMatA.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Cult_Diffuse");
			enemyMatA.NormalTexture = Frosty::AssetManager::GetTexture2D("Cult_defaultMat_Normal");
			m_World->AddComponent<Frosty::ECS::CPhysics>(enemyA, Frosty::AssetManager::GetBoundingBox("Cultist"), enemyATransform.Scale, 6.0f);
			auto& enemyComp = m_World->AddComponent<Frosty::ECS::CEnemy>(enemyA, nullptr, &enemyWeaponCompA, 0.1f);
			enemyComp.SpawnPosition = { 0.0f, 0.0f, 0.0f };
			m_World->AddComponent<Frosty::ECS::CHealth>(enemyA, 10);
			m_World->AddComponent<Frosty::ECS::CHealthBar>(enemyA, glm::vec3(0.0f, 10.0f, 0.0f));
			m_World->AddComponent<Frosty::ECS::CDropItem>(enemyA);
		}
		//enemy wolf
		if (e.GetEntityType() == 23)
		{
			//// ENEMY WEAPON 
			auto& enemyWeaponA = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			//world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("pCube1"));
			//world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("FlatColor"));
			auto& enemyWeaponCompA = m_World->AddComponent<Frosty::ECS::CWeapon>(enemyWeaponA, Frosty::ECS::CWeapon::WeaponType::Bow, 1, 1.0f);
			enemyWeaponCompA.LVL1AttackCooldown = 3.0f;
			enemyWeaponCompA.MaxAttackRange = 5.0f;
			enemyWeaponCompA.MinAttackRange = 0.0f;
			//enemyWeaponCompA.MaxAttackRange = 22.0f;
			//enemyWeaponCompA.MinAttackRange = 18.0f;
			//enemyWeaponCompA.AttackHitboxScale = glm::vec3(10.0f, 6.0f, 4.0f);				// Sword
			enemyWeaponCompA.AttackHitboxScale = glm::vec3(4.0f, 6.0f, 4.0f);				// Bite
			//enemyWeaponCompA.AttackHitboxScale = glm::vec3(0.3f);							// Arrow


			// ENEMY A
			auto& enemyA = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
			auto& enemyATransform = m_World->GetComponent<Frosty::ECS::CTransform>(enemyA);
			//world->AddComponent<Frosty::ECS::CAnimController>(enemyA).currAnim = Frosty::AssetManager::GetAnimation("Wolf_Running");
			m_World->AddComponent<Frosty::ECS::CMesh>(enemyA, Frosty::AssetManager::GetMesh("Wolf"));
			auto& enemyMatA = m_World->AddComponent<Frosty::ECS::CMaterial>(enemyA, Frosty::AssetManager::GetShader("Texture2D"));
			enemyMatA.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Wolf_Diffuse");
			enemyMatA.NormalTexture = Frosty::AssetManager::GetTexture2D("wolf_defaultMat_Normal");
			m_World->AddComponent<Frosty::ECS::CPhysics>(enemyA, Frosty::AssetManager::GetBoundingBox("Wolf"), enemyATransform.Scale, 6.0f);
			auto& enemyComp = m_World->AddComponent<Frosty::ECS::CEnemy>(enemyA, nullptr, &enemyWeaponCompA, 0.0f);
			enemyComp.SpawnPosition = { 0.0f, 0.0f, 0.0f };
			m_World->AddComponent<Frosty::ECS::CHealth>(enemyA, 10);
			m_World->AddComponent<Frosty::ECS::CHealthBar>(enemyA, glm::vec3(0.0f, 10.0f, 0.0f));
			m_World->AddComponent<Frosty::ECS::CDropItem>(enemyA);
		}
	}

	void LevelSystem::OnPlayerUpdateCoordEvent(Frosty::UpdatePlayerRoomCoordEvent& e)
	{
		if (m_PlayerCoords != e.GetCoords())
		{
		//	FY_INFO("Changed Room");
			m_OtherRoom = m_PlayerCoords;
			FlipRoomNames();
			m_World->ChangeCurrentRoom();
			Frosty::EventBus::GetEventBus()->Publish<Frosty::UpdateCurrentRoomEvent>(Frosty::UpdateCurrentRoomEvent(m_T_Room.RoomName, m_T_Room.Rotation));
			Frosty::EventBus::GetEventBus()->Publish<Frosty::SwitchRoomEvent>(Frosty::SwitchRoomEvent());
		}
		m_PlayerCoords = e.GetCoords();
	}

	void LevelSystem::FlipRoomNames()
	{
		RoomInfo temp = m_T_Room;
		m_T_Room = m_O_Room;
		m_O_Room = temp;
	}

	void LevelSystem::OnResetEvent(Frosty::ResetEvent& e)
	{
		Frosty::ECS::CMesh* weaponMesh = nullptr;
		Frosty::ECS::CAnimController* animation = nullptr;
		Frosty::ECS::CPlayer* Player = nullptr;
		Frosty::ECS::CTransform* PlayerT = nullptr;
		Frosty::ECS::CWeapon* Weapon = nullptr;
		Frosty::ECS::CHealth* Health = nullptr;
		Frosty::ECS::CLight* Torch = nullptr;
		int weaponID = 0;

		for (size_t i = 1; i < p_Total; i++)
		{
			if (!m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr))
			{
				if (!m_World->HasComponent<Frosty::ECS::CWeapon>(m_Transform[i]->EntityPtr))
				{
					if (!m_World->HasComponent<Frosty::ECS::CCamera>(m_Transform[i]->EntityPtr))
					{
						if (!m_World->HasComponent<Frosty::ECS::CLight>(m_Transform[i]->EntityPtr))
						{
						}
						else if (m_World->HasComponent<Frosty::ECS::CLight>(m_Transform[i]->EntityPtr))
						{
							auto& light = m_World->GetComponent<Frosty::ECS::CLight>(m_Transform[i]->EntityPtr);
							if (light.Origin != nullptr)
							{
								if (!m_World->HasComponent<Frosty::ECS::CPlayer>(light.Origin->EntityPtr))
								{
									if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
									{
										m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
									}
								}
							}
							else if (light.Origin == nullptr)
							{
								if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
								{
									m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
								}
							}
						}
					}
				}
				else if (!m_World->GetComponent<Frosty::ECS::CWeapon>(m_Transform[i]->EntityPtr).IsPlayerWeapon)
				{
				}
				else if (m_World->GetComponent<Frosty::ECS::CWeapon>(m_Transform[i]->EntityPtr).IsPlayerWeapon)
				{
					auto& weaponHandler = Frosty::AssetManager::GetWeaponHandler("Weapons");
					Frosty::Weapon lootWeaponComp = weaponHandler->GetAPlayerWeapon(1, 1);
					auto& playerWeaponComp = m_World->GetComponent<Frosty::ECS::CWeapon>(m_Transform[i]->EntityPtr);
					Frosty::ECS::CWeapon::WeaponType type = playerWeaponComp.Type;

					if (lootWeaponComp.Type == Frosty::Weapon::WeaponType::Sword)
					{
						playerWeaponComp.Type = Frosty::ECS::CWeapon::WeaponType::Sword;
						auto& mesh = m_World->GetComponent<Frosty::ECS::CMesh>(m_Transform[i]->EntityPtr);
						mesh.Mesh = Frosty::AssetManager::GetMesh("sword");
						auto& weaponMat = m_World->GetComponent<Frosty::ECS::CMaterial>(m_Transform[i]->EntityPtr);
						weaponMat.UseShader = Frosty::AssetManager::GetShader("Texture2D");
						weaponMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("sword_lvl1_diffuse");
						weaponMat.NormalTexture = Frosty::AssetManager::GetTexture2D("sword_normal");

						if (playerWeaponComp.Type == Frosty::ECS::CWeapon::WeaponType::Sword)
						{
							m_GUI = GetPlayerGUI();
							if (m_GUI != nullptr)
							{
								m_GUI->Layout.sprites[2].SetImage("attackMelee");
								m_GUI->Layout.sprites[3].SetImage("attackMelee1");
								m_GUI->Layout.sprites[4].SetImage("attackMelee2");
								m_GUI->Layout.sprites[5].SetImage("attackMelee3");
							}
							Frosty::Renderer::ChangeEntity(m_Transform[i]->EntityPtr->Id, &weaponMat, "Sword", &mesh, m_Transform[i]->EntityPtr->Id, m_Transform[i], nullptr);
						}
						weaponMesh = &mesh;
					}
					else if (lootWeaponComp.Type == Frosty::Weapon::WeaponType::Bow)
					{
						playerWeaponComp.Type = Frosty::ECS::CWeapon::WeaponType::Bow;
						auto& mesh = m_World->GetComponent<Frosty::ECS::CMesh>(m_Transform[i]->EntityPtr);
						mesh.Mesh = Frosty::AssetManager::GetMesh("Bow");
						auto& weaponMat = m_World->GetComponent<Frosty::ECS::CMaterial>(m_Transform[i]->EntityPtr);
						weaponMat.UseShader = Frosty::AssetManager::GetShader("Texture2D");
						weaponMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("bow_lvl1_diffuse");
						weaponMat.NormalTexture = Frosty::AssetManager::GetTexture2D("bow_normal");

						if (playerWeaponComp.Type == Frosty::ECS::CWeapon::WeaponType::Bow)
						{
							m_GUI = GetPlayerGUI();
							if (m_GUI != nullptr)
							{
								m_GUI->Layout.sprites[2].SetImage("attackRanged");
								m_GUI->Layout.sprites[3].SetImage("attackRanged1");
								m_GUI->Layout.sprites[4].SetImage("attackRanged2");
								m_GUI->Layout.sprites[5].SetImage("attackRanged3");
							}
							Frosty::Renderer::ChangeEntity(m_Transform[i]->EntityPtr->Id, &weaponMat, "Bow", &mesh, m_Transform[i]->EntityPtr->Id, m_Transform[i], nullptr);
						}
						weaponMesh = &mesh;
					}

					playerWeaponComp.Level = lootWeaponComp.Level;
					playerWeaponComp.Speciality = lootWeaponComp.Speciality;
					playerWeaponComp.MaxAttackRange = lootWeaponComp.MaxAttackRange;
					playerWeaponComp.MinAttackRange = lootWeaponComp.MinAttackRange;
					playerWeaponComp.Damage = lootWeaponComp.Damage;
					playerWeaponComp.CriticalHit = lootWeaponComp.CriticalHit;
					playerWeaponComp.CriticalHitChance = lootWeaponComp.CriticalHitChance;
					playerWeaponComp.LVL1AttackCooldown = lootWeaponComp.LVL1AttackCooldown;
					playerWeaponComp.LVL2AttackCooldown = lootWeaponComp.LVL2AttackCooldown;
					playerWeaponComp.LVL3AttackCooldown = lootWeaponComp.LVL3AttackCooldown;
					playerWeaponComp.Lifetime = lootWeaponComp.Lifetime;
					playerWeaponComp.AttackHitboxScale = lootWeaponComp.AttackHitboxScale;
					playerWeaponComp.FireCriticalHitChance = 0;
					playerWeaponComp.EarthDamage = 0;
					playerWeaponComp.WindSpeed = 0;
					playerWeaponComp.WaterHealing = 0;
					playerWeaponComp.IsFullyUpgraded = false;
					playerWeaponComp.ProjectileSpeed = lootWeaponComp.ProjectileSpeed;

					weaponID = (int)m_Transform[i]->EntityPtr->Id;
				}
			}
			else if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr))
			{
				auto& playerTransform = m_World->GetComponent<Frosty::ECS::CTransform>(m_Transform[i]->EntityPtr);
				Player = &m_World->GetComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr);
				playerTransform.Position = glm::vec3(-104.0f, 0.0f, -15.4f);
				animation = &m_World->GetComponent<Frosty::ECS::CAnimController>(m_Transform[i]->EntityPtr);
				PlayerT = &playerTransform;
				auto& health = m_World->GetComponent<Frosty::ECS::CHealth>(m_Transform[i]->EntityPtr);
				int healthSpriteCounter = health.MaxPossibleHealth;
				health.CurrentHealth = 20;
				health.MaxHealth = 20;
				health.MaxPossibleHealth = 40;
				Player->Score = 0;

				auto& playerInventory = m_World->GetComponent<Frosty::ECS::CInventory>(m_Transform[i]->EntityPtr);
				playerInventory.MaxHealingPotions = 5;
				playerInventory.CurrentHealingPotions = 0;
				playerInventory.MaxIncreaseHPPotions = 5;
				playerInventory.CurrentIncreaseHPPotions = 0;
				playerInventory.MaxSpeedPotions = 5;
				playerInventory.CurrentSpeedPotions = 0;
				playerInventory.MaxSpeedBoots = 5;
				playerInventory.CurrentSpeedBoots = 0;
				playerInventory.MaxBaitAmount = 5;
				playerInventory.CurrentBaitAmount = 0;
				playerInventory.MaxWolfsbaneAmount = 10;
				playerInventory.CurrentWolfsbane = 0;

				m_GUI = &m_World->GetComponent<Frosty::ECS::CGUI>(m_Transform[i]->EntityPtr);

				for (int j = 15; j < 20 + (healthSpriteCounter / 4); j++)
				{
					m_GUI->Layout.sprites.at(j).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));
				}
			}
		}

		weaponMesh->parentMatrix = PlayerT->GetModelMatrix();
		animation->holdPtr = animation->currAnim->getHoldingJoint();
		weaponMesh->animOffset = animation->holdPtr;
		Frosty::Renderer::UpdateCMesh(weaponID, weaponMesh);

		m_BossSpawned = false;
		m_BossRoomTimer = 2.0f;
		m_BossTimer = 0.0f;
		m_PlayerCoords = { 10, 15 };
		m_Start = true;

		m_World->DestroyGroup(true);

		m_ReStart = true;
	}

	Frosty::ECS::CGUI* LevelSystem::GetPlayerGUI()
	{
		for (int i = 1; i < p_Total; i++)
		{
			if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr))
			{
				m_GUI = &m_World->GetComponent<Frosty::ECS::CGUI>(m_Transform[i]->EntityPtr);
			}
		}
		return m_GUI;
	}

	void LevelSystem::OnBossSpawnedEvent(Frosty::BossSpawnedEvent& e)
	{
		m_BossSpawned = true;
	}

	void LevelSystem::OnBaitPlacedEvent(Frosty::BaitPlacedEvent& e)
	{
		auto& baitTransform = m_World->GetComponent<Frosty::ECS::CTransform>(e.GetEntity());
		if (m_LevelFileFormat.AddBaitToMap(baitTransform.Position, m_PlayerCoords))
		{
			FY_INFO("bait is now saved");
		}
		else
		{
			FY_INFO("culdent save bait");
		}
		bool alradyExist = false;
		for (int i = 0; i < m_RoomswhithBait.size(); i++)
		{
			if (m_RoomswhithBait[i] == m_PlayerCoords)
				alradyExist = true;
		}
		if (!alradyExist)
		{
			m_RoomswhithBait.push_back(m_PlayerCoords);
		}
	}

	void LevelSystem::randomBossMovment()
	{
		Room bossCurrentRoom = m_Map.getRoom(m_BossPos);
		int roomToEnter;
		bool redoRoom = false;
		glm::ivec2 tempLastPos = m_BossPos;
		glm::ivec2 tempPos = m_BossPos;
		do
		{
			redoRoom = false;
			roomToEnter = rand() % 4;
			if (bossCurrentRoom.sideExits[roomToEnter])
			{
				tempLastPos = m_BossPos;
				tempPos = m_BossPos;
				if (roomToEnter == 0)
					tempPos += glm::ivec2(0, -1);
				if (roomToEnter == 1)
					tempPos += glm::ivec2(0, 1);
				if (roomToEnter == 2)
					tempPos += glm::ivec2(-1, 0);
				if (roomToEnter == 3)
					tempPos += glm::ivec2(1, 0);
				if (tempPos == m_BossLastRoom)
				{
					FY_INFO("The boss want's to go back{0}", "!");
					int chansToReturn = rand() % 4;
					if (chansToReturn != 1)
					{
						redoRoom = true;
						FY_INFO("The boss faild to move back :(");
					}
					else
						FY_INFO("The boss succeded in moving back :)");
				}

				Room tempChek = m_Map.getRoom(tempPos);
				if (!tempChek.Ocupide)
				{
					FY_INFO("wath how? don't do this");
					redoRoom = true;
				}
			}
			else
			{
				FY_INFO("The boss tryed to move but faild! ({0}, {1})", m_BossPos.x, m_BossPos.y);
			}
		} while (!bossCurrentRoom.sideExits[roomToEnter] || redoRoom);
		m_BossPos = tempPos;
		m_BossLastRoom = tempLastPos;
		FY_INFO("The boss is moving to ({0}, {1})", m_BossPos.x, m_BossPos.y);
	}
}
