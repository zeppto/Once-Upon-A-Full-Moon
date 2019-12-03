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

		//m_LevelFileFormat.LoadBoolMap("deadend_chests_IsStatick_t_p_e_r_h");
		// "crossroad_chests_IsStatick_t_p_e_r_h";
		// "threeWayRoad_chests_IsStatick_t_p_e_r_h";
		// "turningRoad_chests_IsStatick_t_p_e_r_h";
		// "straightRoad_chests_IsStatick_t_p_e_r_h";
		// "deadend_chests_IsStatick_t_p_e_r_h";

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

			//int rotation = 0;
			//std::string texture = m_Map.getRoomTextur(m_PlayerPos, &rotation);
			//Level::Room(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1], m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3], texture, rotation);
			//this is curently the start room
			Frosty::ECS::CTransform* playerTransform = nullptr;
			for (size_t i = 1; i < p_Total; i++)
			{
				if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_Transform[i]->EntityPtr))
				{
					playerTransform = m_Transform[i];
				}
			}
			int rotate;
			m_Map.getRoomTextur(m_PlayerCoords, &rotate);
			m_CurrentRoomBool = m_World->GetCurrentRoom();
			//	Level::MoveToNewRoom(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1], m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3]);
			m_LevelFileFormat.OpenFromFile("deadend_chests_IsStatick_t_p_e_r_h", !m_CurrentRoomBool, m_PlayerCoords, playerTransform, rotate);
			
			m_T_Room.RoomName = "deadend_chests_IsStatick_t_p_e_r_h";

			auto& jj = Frosty::AssetManager::GetBoolMap("deadend_chests_IsStatick_t_p_e_r_h");

			m_T_Room.Rotation = rotate;
			Frosty::EventBus::GetEventBus()->Publish<Frosty::UpdateCurrentRoomEvent>(Frosty::UpdateCurrentRoomEvent(m_T_Room.RoomName, m_T_Room.Rotation));


			//m_RoomRotation = rotate;
			//m_ThisRoomName = "deadend_chests_IsStatick_t_p_e_r_h";
			m_Start = false;

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
		auto& ExitTranform = m_World->GetComponent<Frosty::ECS::CTransform>(e.GetExitEntity());
		//auto& ExitBBox = m_World->GetComponent<Frosty::ECS::CPhysics>(e.GetExitEntity());
		auto& ExitSide = m_World->GetComponent<Frosty::ECS::CLevelExit>(e.GetExitEntity());

		auto& PlayerTranform = m_World->GetComponent<Frosty::ECS::CTransform>(e.GetPlayerEntity());
		Frosty::ECS::CTransform* playerTransform = nullptr;

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
			m_PlayerTransformLoadComponent = playerTransform;
			m_LoadFileName = m_Map.getRoomTextur(m_OtherRoom, &m_MapRotation);
			//PlayerTranform.Position = Level::Room(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1], 
			//	m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3], texture, rotation, ExitSide.ExitDirection);
			//PlayerTranform.Position = Level::MoveToNewRoom(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1],
			//	m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3], ExitSide.ExitDirection);
			//m_LevelFileFormat.OpenFromFile(fileName, m_PlayerCoords, playerTransform, rotation);

		}

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
			m_World->AddComponent<Frosty::ECS::CMesh>(enemy, Frosty::AssetManager::GetMesh("pCube1"));
			m_World->AddComponent<Frosty::ECS::CMaterial>(enemy, Frosty::AssetManager::GetShader("FlatColor"));
			m_World->AddComponent<Frosty::ECS::CPhysics>(enemy, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);
			m_World->AddComponent<Frosty::ECS::CEnemy>(enemy);
			//m_World->AddComponent<Frosty::ECS::CFollow>(enemy);
			m_World->AddComponent<Frosty::ECS::CHealth>(enemy);
		}
		//Stone
		if (e.GetEntityType() == 1)
		{
			auto& stone = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 90.0f }, { 3.0f, 3.0f, 3.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(stone, Frosty::AssetManager::GetMesh("stone1"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(stone, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("stoneTexture2");
			m_World->AddComponent<Frosty::ECS::CPhysics>(stone, Frosty::AssetManager::GetBoundingBox("stone1"), 0.0f);
		}
		//Tree 
		if (e.GetEntityType() == 2)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("tree1"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("tree1"), 0.0f);
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
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("mushroomsAndStonesBig"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("stoneAndMushRooms");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("mushroomsAndStonesBig"), 0.0f);
		}
		//mushroomsAndStonesSmall
		if (e.GetEntityType() == 6)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("mushroomsAndStonesSmall"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("stoneAndMushRooms");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("mushroomsAndStonesSmall"), 0.0f);
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
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("stones2"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("stoneTexture2");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("stones2"), 0.0f);
		}
		//3stones
		if (e.GetEntityType() == 11)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("stones3"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("stoneTexture2");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("stones3"), 0.0f);
		}
		//stones4
		if (e.GetEntityType() == 12)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("stones4"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("stoneTexture2");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("stones4"), 0.0f);
		}
		//treeBunch3
		if (e.GetEntityType() == 13)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("treeBunch3"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("treeBunch3"), 0.0f);
		}
		//treeBunch4
		if (e.GetEntityType() == 14)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("treeBunch4"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("treeBunch4"), 0.0f);
		}
		//treeBunch7
		if (e.GetEntityType() == 15)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("treeBunch7"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("treeBunch7"), 0.0f);
		}
		//treeBunchWall
		if (e.GetEntityType() == 16)
		{
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("treeBunchWall"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("treeBunchWall"), 0.0f);
		}
		//chest
		if (e.GetEntityType() == 17)
		{
			auto& chest = m_World->CreateEntity({ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(chest, Frosty::AssetManager::GetMesh("chest"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(chest, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("chest");
			m_World->AddComponent<Frosty::ECS::CPhysics>(chest, Frosty::AssetManager::GetBoundingBox("chest"), 6.0f);
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
			//world->AddComponent<Frosty::ECS::CAnimController>(enemyA).currAnim = Frosty::AssetManager::GetAnimation("Wolf_Running");
			m_World->AddComponent<Frosty::ECS::CMesh>(enemyA, Frosty::AssetManager::GetMesh("Cultist"));
			auto& enemyMatA = m_World->AddComponent<Frosty::ECS::CMaterial>(enemyA, Frosty::AssetManager::GetShader("Texture2D"));
			enemyMatA.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Cult_Diffuse");
			enemyMatA.NormalTexture = Frosty::AssetManager::GetTexture2D("Cult_defaultMat_Normal");
			m_World->AddComponent<Frosty::ECS::CPhysics>(enemyA, Frosty::AssetManager::GetBoundingBox("Cultist"), 6.0f);
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
			//world->AddComponent<Frosty::ECS::CAnimController>(enemyA).currAnim = Frosty::AssetManager::GetAnimation("Wolf_Running");
			m_World->AddComponent<Frosty::ECS::CMesh>(enemyA, Frosty::AssetManager::GetMesh("Wolf"));
			auto& enemyMatA = m_World->AddComponent<Frosty::ECS::CMaterial>(enemyA, Frosty::AssetManager::GetShader("Texture2D"));
			enemyMatA.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Wolf_Diffuse");
			enemyMatA.NormalTexture = Frosty::AssetManager::GetTexture2D("wolf_defaultMat_Normal");
			m_World->AddComponent<Frosty::ECS::CPhysics>(enemyA, Frosty::AssetManager::GetBoundingBox("Wolf"), 6.0f);
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
			FY_INFO("Changed Room");
			m_OtherRoom = m_PlayerCoords;
			FlipRoomNames();
			m_World->ChangeCurrentRoom();
			Frosty::EventBus::GetEventBus()->Publish<Frosty::UpdateCurrentRoomEvent>(Frosty::UpdateCurrentRoomEvent(m_T_Room.RoomName, m_T_Room.Rotation));
		}
		m_PlayerCoords = e.GetCoords();
	}

	void LevelSystem::FlipRoomNames()
	{
		RoomInfo temp = m_T_Room;
		m_T_Room = m_O_Room;
		m_O_Room = temp;
	}

}


