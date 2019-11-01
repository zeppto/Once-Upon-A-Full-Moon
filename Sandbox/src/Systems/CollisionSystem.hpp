#ifndef COLLISION_SYSTEM_HPP
#define COLLISION_SYSTEM_HPP
#include "Frosty.h"

class CollisionSystem : public Frosty::ECS::BaseSystem
{
public:
	CollisionSystem() = default;
	virtual ~CollisionSystem() = default;

	virtual void Init() override;

	virtual void OnUpdate() override;

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

private:
	std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
	std::array<Frosty::ECS::CCollision*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Collision;

};

#endif // !COLLISION_SYSTEM_HPP
