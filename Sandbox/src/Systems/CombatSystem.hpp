#ifndef COMBATSYSTEM_HPP
#define COMBATSYSTEM_HPP
#include "Frosty.h"

class CombatSystem : public Frosty::ECS::BaseSystem
{
public:
	CombatSystem() = default;
	virtual ~CombatSystem() = default;

	virtual void Init() override;

	virtual void OnInput() override;

	virtual void OnUpdate() override;

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

private:
	std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
	std::array<Frosty::ECS::CCollision*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Collision;
	std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;
	std::array<Frosty::ECS::CTag*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Tag;
	//
	std::array<Frosty::ECS::CArrow*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Arrow;
};

#endif // !COMBATSYSTEM_HPP