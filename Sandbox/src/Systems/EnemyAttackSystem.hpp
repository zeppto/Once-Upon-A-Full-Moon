#ifndef ENEMY_ATTACK_SYSTEM_HPP
#define ENEMY_ATTACK_SYSTEM_HPP
#include "Frosty.h"

class EnemyAttackSystem : public Frosty::ECS::BaseSystem
{
public:
	EnemyAttackSystem() = default;
	virtual ~EnemyAttackSystem() = default;

	virtual void Init() override;

	virtual void OnInput() override;

	virtual void OnUpdate() override;

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

private:
	std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
	std::array<Frosty::ECS::CCollision*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Collision;
	std::array<Frosty::ECS::CEnemyAttack*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_EnemyAttack;
	std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;

};

#endif // !ENEMY_ATTACK_SYSTEM_HPP