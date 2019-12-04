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
		if (m_CreatNewRoom)
		{
			int rotation = 0;
			std::string fileName = m_Map.getRoomTextur(m_PlayerPos, &rotation);
			//PlayerTranform.Position = Level::MoveToNewRoom(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1],
			//	m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3], ExitSide.ExitDirection);
			m_LevelFileFormat.OpenFromFile(fileName, m_PlayerPos, m_PlayerTransform, rotation);
			m_CreatNewRoom = false;
		}
		if (m_LodeNamedRoom)
		{
			m_LevelFileFormat.OpenFromFile(m_RoomType, m_PlayerPos);
			m_LodeNamedRoom = false;
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
		case Frosty::EventType::Reset:
			OnResetEvent(static_cast<Frosty::ResetEvent&>(e));
			break;
		case Frosty::EventType::BossSpawned:
			OnBossSpawnedEvent(static_cast<Frosty::BossSpawnedEvent&>(e));
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
			m_CurrentRoome = m_Map.getRoom(m_PlayerPos);

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
			m_Map.getRoomTextur(m_PlayerPos, &rotate);
			Level::MoveToNewRoom(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1], m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3]);
			m_LevelFileFormat.OpenFromFile("deadend_chests_IsStatick_t_p_e_r_h_a", m_PlayerPos, playerTransform, rotate);
			m_Start = false;
			m_LevelFileFormat.LoadBoolMap("deadend_chests_IsStatick_t_p_e_r_h_a");
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
		Frosty::ECS::CTransform* playerTransform = nullptr;

		//temp level swap
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Transform[i]->EntityPtr != nullptr)
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
									if (!m_World->HasComponent<Frosty::ECS::CPlayer>(light.Origin->EntityPtr))
									{
										if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
										{
											m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
										}
									}
							}
						}
					}
					else
					{
						playerTransform = m_Transform[i];
					}
				}
			}
		}
		if (ExitSide.ExitDirection == 0)
			m_PlayerPos += glm::ivec2(0, -1);
		if (ExitSide.ExitDirection == 1)
			m_PlayerPos += glm::ivec2(0, 1);
		if (ExitSide.ExitDirection == 2)
			m_PlayerPos += glm::ivec2(-1, 0);
		if (ExitSide.ExitDirection == 3)
			m_PlayerPos += glm::ivec2(1, 0);

		m_CurrentRoome = m_Map.getRoom(m_PlayerPos);
		//m_EntrensSide = ExitSide.ExitDirection;
		//m_NextLevel = true;
		//m_TempTimer = 0;

		m_PlayerTransform = playerTransform;

		//int rotation = 0;
		//std::string fileName = m_Map.getRoomTextur(m_PlayerPos, &rotation);
		//PlayerTranform.Position = Level::Room(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1], 
		//	m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3], texture, rotation, ExitSide.ExitDirection);
		PlayerTranform.Position = Level::MoveToNewRoom(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1],
			m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3], ExitSide.ExitDirection);
		//m_LevelFileFormat.OpenFromFile(fileName, m_PlayerPos, playerTransform, rotation);
		m_CreatNewRoom = true;

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
		//m_LevelFileFormat.OpenFromFile(m_RoomType, m_PlayerPos, playerTransform);
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

	void LevelSystem::OnResetEvent(Frosty::ResetEvent & e)
	{
		Frosty::ECS::CMesh* weaponMesh = nullptr;
		Frosty::ECS::CAnimController* animation = nullptr;
		Frosty::ECS::CPlayer* Player = nullptr;
		Frosty::ECS::CTransform* PlayerT = nullptr;
		Frosty::ECS::CWeapon* m_Weapon = nullptr;
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
							if (!m_World->HasComponent<Frosty::ECS::CGUI>(m_Transform[i]->EntityPtr))
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
					if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr))
					{
						m_World->AddComponent<Frosty::ECS::CDestroy>(m_Transform[i]->EntityPtr);
					}
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
								m_GUI->Layout.sprites.at(1).SetImage("attackMelee");
								m_GUI->Layout.sprites.at(2).SetImage("attackMelee1");
								m_GUI->Layout.sprites.at(3).SetImage("attackMelee2");
								m_GUI->Layout.sprites.at(4).SetImage("attackMelee3");
							}
							Frosty::Renderer::ChangeEntity(m_Transform[i]->EntityPtr->Id, &weaponMat, "Sword", &mesh, m_Transform[i]->EntityPtr->Id, m_Transform[i]);
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
								m_GUI->Layout.sprites.at(1).SetImage("attackRanged");
								m_GUI->Layout.sprites.at(2).SetImage("attackRanged1");
								m_GUI->Layout.sprites.at(3).SetImage("attackRanged2");
								m_GUI->Layout.sprites.at(4).SetImage("attackRanged3");
							}
							Frosty::Renderer::ChangeEntity(m_Transform[i]->EntityPtr->Id, &weaponMat, "Bow", &mesh, m_Transform[i]->EntityPtr->Id, m_Transform[i]);
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

					weaponID = m_Transform[i]->EntityPtr->Id;
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

				m_GUI->Layout.sprites.at(14).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));
				m_GUI->Layout.sprites.at(15).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));
				m_GUI->Layout.sprites.at(16).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));
				m_GUI->Layout.sprites.at(17).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));
				m_GUI->Layout.sprites.at(18).SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));
			}
		}

		weaponMesh->parentMatrix = PlayerT->GetModelMatrix();
		animation->holdPtr = animation->currAnim->getHoldingJoint();
		weaponMesh->animOffset = animation->holdPtr;
		Frosty::Renderer::UpdateCMesh(weaponID, weaponMesh);

		m_Start = true;
		m_PlayerPos = { 10, 15 };
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
}
