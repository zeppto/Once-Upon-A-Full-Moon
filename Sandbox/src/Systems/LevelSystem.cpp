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
			int rotate;
			m_Map.getRoomTextur(m_PlayerPos, &rotate);
			Level::MoveToNewRoom(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1], m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3]);
			m_LevelFileFormat.OpenFromFile("deadend_chests_IsStatick_t", m_PlayerPos, nullptr, rotate);
			m_Start = false;
			m_LevelFileFormat.LoadBoolMap("deadend_chests_IsStatick_t");
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
		Frosty::ECS::CTransform* playerTransform;

		//temp level swap
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Transform[i]->EntityPtr != nullptr)
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

		int rotation = 0;
		std::string fileName = m_Map.getRoomTextur(m_PlayerPos, &rotation);
		//PlayerTranform.Position = Level::Room(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1], 
		//	m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3], texture, rotation, ExitSide.ExitDirection);
		PlayerTranform.Position = Level::MoveToNewRoom(m_CurrentRoome.sideExits[0], m_CurrentRoome.sideExits[1],
			m_CurrentRoome.sideExits[2], m_CurrentRoome.sideExits[3], ExitSide.ExitDirection);
		m_LevelFileFormat.OpenFromFile(fileName, m_PlayerPos, playerTransform, rotation);

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
						if (m_World->HasComponent<Frosty::ECS::CPhysics>(m_Transform[i]->EntityPtr))
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
						if (m_World->HasComponent<Frosty::ECS::CPhysics>(m_Transform[i]->EntityPtr))
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
		m_LevelFileFormat.OpenFromFile(m_RoomType, m_PlayerPos, playerTransform);
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
			auto& mushroom = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
			m_World->AddComponent<Frosty::ECS::CMesh>(mushroom, Frosty::AssetManager::GetMesh("hexCircle"));
			auto& material = m_World->AddComponent<Frosty::ECS::CMaterial>(mushroom, Frosty::AssetManager::GetShader("Texture2D"));
			material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("mashRoomCirkel");
			m_World->AddComponent<Frosty::ECS::CPhysics>(mushroom, Frosty::AssetManager::GetBoundingBox("hexCircle"), 0.0f);
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
			m_World->AddComponent<Frosty::ECS::CHealth>(chest, 2.0f);
			m_World->AddComponent<Frosty::ECS::CDropItem>(chest);
		}
	}
}


