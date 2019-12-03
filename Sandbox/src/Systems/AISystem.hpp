#ifndef AI_SYSTEM_HPP
#define AI_SYSTEM_HPP

namespace Frosty { class InitiateGridEvent; class ResetBossAbilitiesEvent; class SpawnBossEvent;}

namespace MCS
{
	class AISystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		AISystem() = default;
		virtual ~AISystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;
		void OnEvent(Frosty::BaseEvent& e);

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity> & entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity> & entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity> & entity) override;
		virtual std::string GetInfo() const override;

	private:
		void OnInitiateGridMap(Frosty::InitiateGridEvent& e);
		void OnResetBossAbilities(Frosty::ResetBossAbilitiesEvent& e);
		void OnSpawnBossEvent(Frosty::SpawnBossEvent& e);
		void CheckState(size_t index);
		void HandleAttack(size_t index);
		void LookAtPoint(const glm::vec3& point, size_t index);
		bool HandleBossAbilities(size_t index);
		void SpawnBoss();
		void ResetBossAbilities(size_t index);

	private:
		Frosty::World* m_World{ nullptr };

		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CEnemy*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Enemy;
		std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;

		// Temporary for extern test
		bool m_BossSpawned{ false };
		float BossSpawnTime{ 20.f };
		float BossTimer{ Frosty::Time::CurrentTime() };
		glm::vec3 m_BossSpawn{ 0.0f };
		Frosty::ECS::CTransform* m_PlayerTransform;
	};
}

#endif // !AI_SYSTEM_HPP
