#include <fypch.hpp>
#include <mcspch.hpp>
#include "SpawnSystem.hpp"

void SpawnSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTag>(), true);
}

void SpawnSystem::OnInput()
{
	if (Frosty::InputManager::IsKeyPressed(FY_KEY_P))	// Damage+ Player
	{
		bool found = false;
		for (size_t i = 1; i < p_Total && !found; i++)
		{
			if (m_Health[i]->EntityPtr == m_Player)
			{
				m_Health[i]->CurrentHealth -= 0.5f;
				found = true;
			}
		}
	}
	else if (Frosty::InputManager::IsKeyPressed(FY_KEY_O))	// Damage+ Opponent
	{
		bool found = false;
		for (size_t i = 1; i < p_Total && !found; i++)
		{
			if (m_Health[i]->EntityPtr != m_Player)
			{
				m_Health[i]->CurrentHealth -= 0.5f;
				found = true;
			}
		}
	}
}

void SpawnSystem::OnUpdate()
{
	//// Initiate once, shouldn't be in update loop
		//if (m_Player == nullptr)
		//{
		//	for (size_t i = 1; i < p_Total; i++)
		//	{
		//		if (m_Health[i]->EntityPtr->Bitset[7] == 1)
		//		{
		//			m_Player = m_Health[i]->EntityPtr;
		//		}
		//	}
		//}

		// If dead --> respawn
	if (m_Player == nullptr && (std::clock() - m_PlayerSpawnTimer) >= 2000)
	{
		SpawnPlayer();
	}
	else if ((p_Total - 2) < m_NrOfEnemies && (std::clock() - m_EnemySpawnTimer) >= 2000)
	{
		SpawnEnemy();
	}

	// If health below 0 --> remove
	for (size_t i = 1; i < p_Total; i++)
	{
		if (m_Health[i]->CurrentHealth <= 0.f && m_Health[i]->EntityPtr == m_Player)
		{
			RemovePlayer();
		}
		else if (m_Health[i]->CurrentHealth <= 0.f)
		{
			RemoveEnemyAt(i);
		}
	}
}

void SpawnSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
	{
		p_EntityMap.emplace(entity, p_Total);

		auto& world = Frosty::Application::Get().GetWorld();
		m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);
		m_Tag[p_Total] = &world->GetComponent<Frosty::ECS::CTag>(entity);

		p_Total++;
	}
}

void SpawnSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

	if (tempIndex > 0)
	{
		p_Total--;
		m_Health[p_Total] = nullptr;

		if (p_Total > 1)
		{
			//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

			if (p_Total > tempIndex)
			{
				std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Health[p_EntityMap[entity]]->EntityPtr;
				p_EntityMap[entityToUpdate] = tempIndex;
			}
		}

		p_EntityMap.erase(entity);
	}
}

void SpawnSystem::SpawnPlayer()
{
	auto& world = Frosty::Application::Get().GetWorld();
	auto& player = world->CreateEntity();
	auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
	playerTransform.Position = glm::vec3(0.0f, 1.0f, 0.0f);
	playerTransform.Scale *= 2.2f;
	world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("Scarlet"));
	world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("Texture2D"));
	world->GetComponent<Frosty::ECS::CMaterial>(player).DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
	world->GetComponent<Frosty::ECS::CMaterial>(player).SpecularTexture = Frosty::AssetManager::GetTexture2D("Scarlet_specular");
	world->GetComponent<Frosty::ECS::CMaterial>(player).NormalTexture = Frosty::AssetManager::GetTexture2D("Scarlet_normal");
	//world->GetComponent<Frosty::ECS::CMaterial>(player).TextureScale = playerTransform.Scale;
	world->AddComponent<Frosty::ECS::CMotion>(player, 20.0f);
	world->AddComponent<Frosty::ECS::CController>(player);
	world->AddComponent<Frosty::ECS::CConsumables>(player);
	world->AddComponent<Frosty::ECS::CHealth>(player);
	world->AddComponent<Frosty::ECS::CTag>(player, "Player");
	world->AddComponent<Frosty::ECS::CPlayerAttack>(player, 1.5f, 1.0f, 2.0f, true);
	world->AddComponent<Frosty::ECS::CEnemyAttack>(player, 1.0f, 1.0f, 2.0f, true);
	world->AddComponent<Frosty::ECS::CHealthBar>(player, glm::vec3(0.0f, 10.0f, 0.0f));

	world->AddComponent<Frosty::ECS::CCollision>(player, Frosty::AssetManager::GetBoundingBox("Cube"));
	auto& gameCameraEntity = world->GetSceneCamera();
	world->GetComponent<Frosty::ECS::CCamera>(gameCameraEntity).Target = &playerTransform;

	m_Player = player;
}

void SpawnSystem::SpawnEnemy()
{
	auto& world = Frosty::Application::Get().GetWorld();
	auto& enemy = world->CreateEntity();
	auto& enemyTransform = world->GetComponent<Frosty::ECS::CTransform>(enemy);

	if (m_KillCount < 0)
	{
		enemyTransform.Position = GetSpawnPosition();
		enemyTransform.Scale *= 2.f;
		world->AddComponent<Frosty::ECS::CMesh>(enemy, Frosty::AssetManager::GetMesh("Cube"));
		world->AddComponent<Frosty::ECS::CMaterial>(enemy, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CMotion>(enemy, 15.0f);
		world->AddComponent<Frosty::ECS::CFollow>(enemy);
		world->AddComponent<Frosty::ECS::CHealth>(enemy);
		world->AddComponent<Frosty::ECS::CCollision>(enemy, Frosty::AssetManager::GetBoundingBox("Cube"));
		world->AddComponent<Frosty::ECS::CTag>(enemy, "Enemy");
		//temp
		world->AddComponent<Frosty::ECS::CPlayerAttack>(enemy);
		world->AddComponent<Frosty::ECS::CEnemyAttack>(enemy);
		world->AddComponent<Frosty::ECS::CArrow>(enemy);
		world->AddComponent<Frosty::ECS::CHealthBar>(enemy);
	}
	else
	{
		enemyTransform.Position = glm::vec3((float)TERRAIN_WIDTH * 6.f, 2.f, 0.f);
		enemyTransform.Scale *= 6.f;
		world->AddComponent<Frosty::ECS::CMesh>(enemy, Frosty::AssetManager::GetMesh("Cube"));
		world->AddComponent<Frosty::ECS::CMaterial>(enemy, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CMotion>(enemy, 19.0f);
		world->AddComponent<Frosty::ECS::CFollow>(enemy);
		world->AddComponent<Frosty::ECS::CHealth>(enemy, 30.0f);
		world->AddComponent<Frosty::ECS::CCollision>(enemy, Frosty::AssetManager::GetBoundingBox("Cube"));
		world->AddComponent<Frosty::ECS::CTag>(enemy, "Enemy");
		//temp
		world->AddComponent<Frosty::ECS::CPlayerAttack>(enemy);
		world->AddComponent<Frosty::ECS::CEnemyAttack>(enemy);
		world->AddComponent<Frosty::ECS::CArrow>(enemy);
		world->AddComponent<Frosty::ECS::CHealthBar>(enemy);

		m_NrOfEnemies = 1;
	}

	if (m_Player != nullptr)
	{
		auto& comp = world->GetComponent<Frosty::ECS::CFollow>(enemy);
		comp.Target = &world->GetComponent<Frosty::ECS::CTransform>(m_Player);
	}

	m_EnemySpawnTimer = float(std::clock());
}

void SpawnSystem::RemovePlayer()
{
	auto& world = Frosty::Application::Get().GetWorld();
	world->RemoveEntity(m_Player);
	m_Player = nullptr;

	RemoveAllEnemies();

	m_KillCount = 0;
	m_NrOfEnemies = 2;
	m_PlayerSpawnTimer = float(std::clock());
}

void SpawnSystem::RemoveEnemyAt(size_t position)
{
	m_Entity = m_Health[position]->EntityPtr;
	auto& world = Frosty::Application::Get().GetWorld();

	FY_TRACE("TAG ON DEAD " + m_Tag[position]->TagName);
	if (m_Tag[position]->TagName == "Enemy")
	{
		////ful fix (när en fiende dör av en pil försvinder båda vilket buggar spelet)
		//if (m_Health[position]->MaxHealth == 0)
		//{
			m_EnemySpawnTimer = float(std::clock());
			m_KillCount++;
			m_NrOfEnemies--;

			if (m_NrOfEnemies == 0)
			{
				m_NrOfEnemies = m_KillCount + 1;
			}

			world->RemoveEntity(m_Entity);
		//}
		//else
		//	m_Health[position]->MaxHealth = 0;
	}
	else
	{
		world->RemoveEntity(m_Entity);
	}
}

void SpawnSystem::RemoveAllEnemies()
{
	for (size_t i = 1; i < p_Total; i++)
	{
		if (m_Health[i]->EntityPtr)
		{
			m_Entity = m_Health[i]->EntityPtr;
			auto& world = Frosty::Application::Get().GetWorld();
			world->RemoveEntity(m_Entity);
			i = 0;
		}
	}
	m_EnemySpawnTimer = float(std::clock());
}

const glm::vec3 SpawnSystem::GetSpawnPosition()
{
	float x = float(rand() % (2 * TERRAIN_WIDTH) - TERRAIN_WIDTH);
	float z = float(rand() % TERRAIN_HEIGHT - float(TERRAIN_HEIGHT * 0.5f));

	return glm::vec3(x, 1.5f, z);
}