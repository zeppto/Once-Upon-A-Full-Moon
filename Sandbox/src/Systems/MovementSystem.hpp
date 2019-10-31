#ifndef MOVEMENT_SYSTEM_HPP
#define MOVEMENT_SYSTEM_HPP
#include "Frosty.h"

class MovementSystem : public Frosty::ECS::BaseSystem
{
public:
	MovementSystem() = default;
	virtual ~MovementSystem() = default;

	virtual void Init() override;

	virtual void OnUpdate() override;

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

private:
	std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
	std::array<Frosty::ECS::CMotion*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Motion;
	std::array<Frosty::ECS::CCollision*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Collision;
	//Temp
	std::array<Frosty::ECS::CTag*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Tag;
};

#endif // !MOVEMENT_SYSTEM_HPP
