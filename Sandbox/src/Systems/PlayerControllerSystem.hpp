#ifndef PLAYER_CONTROLLER_SYSTEM_HPP
#define PLAYER_CONTROLLER_SYSTEM_HPP
#include "Frosty.h"
#include "Frosty/Events/CombatEvent.hpp"

class PlayerControllerSystem : public Frosty::ECS::BaseSystem
{
public:
	PlayerControllerSystem() = default;
	virtual ~PlayerControllerSystem() = default;

	virtual void Init() override;

	virtual void OnInput() override;

	virtual void OnUpdate() override;

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

private:
	glm::vec3 ScreenToTerrainPoint();

	void LookAtPoint(const glm::vec3& point, size_t index);

	void CalculateDirection(size_t index);

	bool InputReleased(int keyReleased);
	bool InputPressed(int keyPressed);

private:
	std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
	std::array<Frosty::ECS::CController*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Controller;
	std::array<Frosty::ECS::CMotion*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Motion;
	std::array<Frosty::ECS::CPlayerAttack*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_PlayerAttack;

	//to prevent attacks if the player never releses the butten
	bool m_CanAttackNormal = true;
	bool m_CanAttackArea = true;
	bool m_CanAttackStrong = true;
};

#endif // !PLAYER_CONTROLLER_SYSTEM_HPP
