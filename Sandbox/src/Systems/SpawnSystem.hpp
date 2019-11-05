//#ifndef SPAWN_SYSTEM_HPP
//#define SPAWN_SYSTEM_HPP
//#include "Frosty.h"
//#include "Frosty/API/AssetManager.hpp"
//
//class SpawnSystem : public Frosty::ECS::BaseSystem
//{
//public:
//	SpawnSystem() = default;
//	virtual ~SpawnSystem() = default;
//
//	virtual void Init() override;
//
//	virtual void OnInput() override;	// VERY temporary
//
//	virtual void OnUpdate() override;
//
//	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
//
//	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
//
//private:
//	void SpawnPlayer();
//	void SpawnEnemy();
//	void RemovePlayer();
//	void RemoveEnemyAt(size_t position);
//	void RemoveAllEnemies();
//	const glm::vec3 GetSpawnPosition();
//
//private:
//	std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;
//	std::array<Frosty::ECS::CTag*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Tag;
//
//	std::shared_ptr<Frosty::ECS::Entity> m_Player = nullptr;
//	std::shared_ptr<Frosty::ECS::Entity> m_Entity = nullptr;
//
//	float m_PlayerSpawnTimer;
//	float m_EnemySpawnTimer;
//
//	// Temp
//	int m_KillCount = 0;
//	int m_NrOfEnemies = 2;			// numbers of enemies simultaniously on the terrain. Start with 2 enemies
//	const int TERRAIN_WIDTH = 45;
//	const int TERRAIN_HEIGHT = 89;
//};
//
//#endif // !SPAWN_SYSTEM_HPP
