#ifndef AI_SYSTEM_HPP
#define AI_SYSTEM_HPP

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

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity> & entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity> & entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity> & entity) override;
		virtual std::string GetInfo() const override;

	private:
		void CheckState(size_t index);
		void HandleAttack(size_t index);
		void LookAtPoint(const glm::vec3& point, size_t index);
		bool HandleBossAbilities(size_t index);
		void SpawnBoss();

	private:
		Frosty::World* m_World{ nullptr };

		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CEnemy*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Enemy;
		std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;

		float BossSpawnTime{ 10.f };
		float BossTimer{ Frosty::Time::CurrentTime() };
	};
}

#endif // !AI_SYSTEM_HPP
